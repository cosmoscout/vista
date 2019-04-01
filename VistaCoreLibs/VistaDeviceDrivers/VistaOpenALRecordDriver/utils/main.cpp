#include <iostream>
#include <sndfile.h>
#include <stdlib.h>

#include <fstream>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <vector>

using namespace std;

void usage(char *pname )
{
	cout << pname << " [format-file] [rawfile] [outfile]" << endl;
	exit(0);
}

int getNumChannelsForFormat( const string &format )
{
	if( format == "MONO8" or format == "MONO16" )
		return 1;
	if( format == "STEREO8" or format == "STEREO16" )
		return 2;
	return 0;
}

int getNumberOfBytesPerSample( const std::string &format )
{
	if( format == "MONO8" )
		return 1;
	if( format == "MONO16" or format == "STEREO8" )
		return 2;
	if( format == "STEREO16" )
		return 4;
	return 0;
}

int getsndfileformat( const std::string &format )
{
	if( format == "MONO8" or format == "STEREO8" )
		return SF_FORMAT_PCM_U8;
	if( format == "MONO16" or format == "STEREO16" )
		return SF_FORMAT_PCM_16;
	return 0;
}
void fillInfo( SF_INFO &info, const string &format, int freq )
{
	memset( &info, 0, sizeof( SF_INFO ) );
	info.channels = getNumChannelsForFormat(format);
	info.samplerate = freq;

	info.format = SF_FORMAT_RAW|SF_ENDIAN_CPU|getsndfileformat(format);
}

int main( int argc, char **argv )
{
	if( argc < 3 )
		usage(argv[0]);

	char *formatfile = argv[1];
	char *rawfile    = argv[2];
	char *outfile    = argv[3];

	ifstream in( formatfile );
	if( !in.good() )
	{
		cerr << "could not open formatfile [" << formatfile << "]" << endl;
		return -1;
	}

	string format;
	int frequency;

	in >> format >> frequency;

	SF_INFO rawinfo;

	fillInfo(rawinfo, format, frequency );

	SNDFILE *raw = sf_open( rawfile, SFM_READ, &rawinfo );
	if( !raw )
	{
		cerr << "could not open rawfile [" << rawfile << "]" << endl;
		return -2;
	}

	SF_INFO wavinfo;
	memset(&wavinfo, 0, sizeof(SF_INFO) );

	wavinfo.channels = getNumChannelsForFormat(format);
	wavinfo.samplerate = frequency;
	wavinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_FILE;

	SNDFILE *wav = sf_open( outfile, SFM_WRITE, &wavinfo);
	if( !wav )
	{
		sf_close( raw );
		cerr << "could not open wav file for output." << endl;
		return -2;
	}

	std::vector<short> vsamples( rawinfo.frames );
	sf_readf_short( raw, &vsamples[0], rawinfo.frames );
	sf_writef_short( wav, &vsamples[0], rawinfo.frames );

	sf_close(wav);
	sf_close(raw);

	cout << "DONE" << endl;
	return 0;
}
