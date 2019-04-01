/******************************************************************
*                                                                 *
*       File : spacemouseclient.cpp                                       *
*                                                                 *
*                                                                 *
*       Contact : Ingo Assenmacher (ingo.assenmacher@imag.fr)     *
*                                                                 *
******************************************************************/

#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaDefaultTimerImp.h>


#include <VistaDeviceDriversBase/VistaDriverManager.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>

#include <VistaDataFlowNet/VdfnUtil.h>
#include <VistaDataFlowNet/VdfnObjectRegistry.h>

#include <VistaTools/VistaProgressBar.h>

#include <VistaDataFlowNet/VdfnObjectRegistry.h>
#include <VistaDataFlowNet/VdfnUtil.h>
#include <VistaDataFlowNet/VdfnGraph.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnPersistence.h>

#include <signal.h>
#include <stdlib.h>


class PrintNode : public IVdfnNode
{
public:
	PrintNode()
	: m_pIn( NULL )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<std::string> >);
	}

	bool PrepareEvaluationRun()
	{
		m_pIn = dynamic_cast<TVdfnPort<std::string>*>(GetInPort("in"));
		return GetIsValid();
	}

protected:
	bool DoEvalNode()
	{
		std::cout << m_pIn->GetValue() << std::endl;
		std::cout.flush();

		return true;
	}


	TVdfnPort<std::string> *m_pIn;
};

class DetermineQuit : public IVdfnNode
{
public:
	DetermineQuit()
	: IVdfnNode(),
	  m_pQuitter( NULL ),
	  nTimes(0)
	  {
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<double> >);
	  }


	bool PrepareEvaluationRun()
	{
		m_pQuitter = dynamic_cast<TVdfnPort<double>*>(GetInPort("in"));
		return GetIsValid();
	}

protected:
	bool DoEvalNode()
	{
		if( nTimes++ > 2 )
		{
			std::cout << "CDetermineQuit -- quit" << std::endl;
			return false; // will cause the graph to return false and
							// abort the evaluation
		}
		else
		{
			std::cout << "nTimes = " << nTimes << std::endl;
		}
		return true;
	}


private:
	TVdfnPort<double> *m_pQuitter;
	unsigned int nTimes;
};


bool bDone = false;

void sigc_handler(int mask)
{
	bDone = true;
}


void Usage(const char *progname)
{
	std::cout << progname << " <dfn-xml-file> <driver-name> <vrpn-sensor-address> <ANALOG|BUTTON|TRACKER> <#samples|-1>" << std::endl;
	exit(0);
}


bool SetupSensor( IVistaDeviceDriver *pDriver,
				  IVistaDriverCreationMethod *pM,
		          const std::string &strSensorAddress,
		          const std::string &strSensorType )
{
	VistaDeviceSensor *pTrackerSensor = new VistaDeviceSensor;

	pTrackerSensor->SetMeasureTranscode( pM->GetTranscoderFactoryForSensor(strSensorType)->CreateTranscoder() );
	pTrackerSensor->SetTypeHint( strSensorType );

	unsigned int nId = pDriver->AddDeviceSensor(pTrackerSensor);

	pTrackerSensor->SetSensorName( strSensorAddress );
	if( pDriver->SetupSensorHistory( pTrackerSensor, 10, 500 ) == false ) 
	{
		vstr::err() << "Could not setup sensor history!" << std::endl;
		return false;
	}

	// create a uniform mapping in the sensor mapping aspect

	VistaDriverSensorMappingAspect *pMapping = pDriver->GetAspect< VistaDriverSensorMappingAspect >();

	pMapping->SetSensorId( pMapping->GetTypeId(strSensorType), 0, nId );
	return true;
}


int main(int argc, char **argv)
{
	if(argc < 6)
		Usage(argv[0]);

    signal( SIGINT, sigc_handler );


	const char *pcDfnXml       = argv[1];
	const char *pcDriverName   = argv[2];
	const char *pcSensorAdress = argv[3];
	const char *pcSensorType   = argv[4];
	const char *pcNumSamples   = argv[5];

	int bShowProgress = 0;
	int nNumSamples = atoi(pcNumSamples);
	unsigned int nNum;

	if(nNumSamples < 0)
	{
		nNum = ~0u;
		bShowProgress = 0; // set to 0 for infinite collection
	}
	else
	{
		nNum = nNumSamples;
		if(argc > 6)
			bShowProgress = atoi(argv[6]);
	}

	IVistaTimerImp::SetSingleton( new VistaDefaultTimerImp );

	VistaDriverMap dmap;

	VistaDriverManager mgr(dmap);
	VdfnObjectRegistry reg;

	VistaProgressBar bar(nNum, 1);

	VdfnUtil::InitVdfn(&mgr.GetDriverMap(), &reg);


	VdfnNodeFactory::GetSingleton()->SetNodeCreator( "print", new TVdfnDefaultNodeCreate<PrintNode> );
	VdfnNodeFactory::GetSingleton()->SetNodeCreator( "quitter", new TVdfnDefaultNodeCreate<DetermineQuit> );



	if( mgr.InitPlugin( "VRPN" ) == false ) 
	{
		vstr::err() << "Could not load VRPN driver. Bailing out." << std::endl;
		return -1;
	}


	IVistaDeviceDriver *pSm        = mgr.CreateAndRegisterDriver(pcDriverName, "VRPN");
	IVistaDriverCreationMethod *pM = mgr.GetCreationMethodForClass("VRPN");



	if(pSm == NULL)
	{
		std::cerr << "could not create driver ["
				  << pcDriverName
				  << "] of type VRPN"
				  << std::endl;
		return -2;
	}

	if(pM == NULL)
	{
		std::cerr << "could not create driver  creation method of type VRPN-GENERIC"
				  << std::endl;
		return -3;

	}
	
	SetupSensor(pSm, pM, pcSensorAdress, pcSensorType );

	VdfnGraph *pGraph = NULL;

	

	if( pSm && pSm->Connect() )
	{
		VistaDriverInfoAspect *pInfo
			= dynamic_cast<VistaDriverInfoAspect*>(
					pSm->GetAspectById( VistaDriverInfoAspect::GetAspectId() ) );
		if(pInfo)
			pInfo->GetInfoProps().Print();


		pSm->SetIsEnabled(true);


		mgr.StartAsyncDriverDispatch();

		pGraph = VdfnPersistence::LoadGraph( pcDfnXml, pcDfnXml, true, true );
		if(pGraph)
		{
			VistaTimer timer;
			unsigned int n = 0;

			if(bShowProgress)
				bar.Start();

			while( (n < nNum) && !bDone)
			{
				pSm->SwapSensorMeasures();
				pGraph->EvaluateGraph( timer.GetMicroTime() );
				++n;
				if(bShowProgress)
					bar.Increment();

				VistaTimeUtils::Sleep(1000);
			}
		}


		mgr.StopAsyncDriverDispatch();

	}
	if(bShowProgress)
		bar.Finish(true);

	// we need to make sure the graph is destructed, as some nodes might
	// do some cleanups in the graph destructor
	delete pGraph;
	std::cout << "good bye then." << std::endl;
}
