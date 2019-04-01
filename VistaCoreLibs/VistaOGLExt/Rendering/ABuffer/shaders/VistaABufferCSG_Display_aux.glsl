/*============================================================================*/
/* GLOABAL VARIABLES                                                          */
/*============================================================================*/
int g_aObjekt[4] = int[4](0,0,0,0);
/*============================================================================*/
/* RenderFragment                                                             */
/*============================================================================*/
void OnEnterObjekt( int iObjekt )
{
	++g_aObjekt[iObjekt];
}
void OnExitObjekt( int iObjekt )
{
	--g_aObjekt[iObjekt];
}
bool RenderObjekt( int iObjekt )
{
	return	( g_aObjekt[1] + g_aObjekt[2] == 2	) && 
			( iObjekt == 3 || g_aObjekt[3] < 1 );
			return true;
}