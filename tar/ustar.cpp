#include <cstring>
#include <cstdio>
#include <ctime>
#include <cstddef>
#include <cstdlib>
#include <cassert>

#include <string>
#include <iostream> // used for ::out() diagnostic function
using namespace std;

#include "ustar.h"

#define rstring(name, field) string Header::name() { return string(field, 0, sizeof field); }
#define wstring(name, field) void Header::name(string value) {int textlen = value.length();int n = textlen < sizeof(field) ? textlen : sizeof(field);memcpy(field, value.c_str(), n);memset(field+n, 0, sizeof(field)-textlen);}
#define rint(name, field) unsigned int Header::name() { unsigned int value; sscanf(field, "%o", &value); return value; }
#define wint(name, field) void Header::name(unsigned int value) { snprintf(field, sizeof(field), "%07o", value); }
#define rlong(name, field) unsigned long Header::name() { unsigned int value; sscanf(field, "%o", &value); return value; }
#define wlong(name, field) void Header::name(unsigned int value) { snprintf(field, sizeof(field), "%011o", value); }


namespace Entrity
{
	namespace Ustar
	{
		/* Read current stream, return pointer to parsed header */
		Header * File::readHeader()
		{
			Header * header = new Header(*p_istream);
			size = header->size();
			offset = p_istream->tellg();
			padding = header->padding();
			return header;
		}

		/* move ahead in stream to end of data block for latest-read header */
		void File::next() { if (size && p_istream) p_istream->seekg(offset + size + padding, ios::beg); }

		/* reads size bytes of data, returns char * with NULL at end; 'size' arg should be strlen, not including NULL char. That extra NULL char is the only difference between this and readBin() */
		char * File::readText(unsigned int size, void * dest)
		{
			if (!size) size = this->size;
			if (!dest) dest = malloc(size+1);
			return readBin(size, dest);
		}

		/* reads size bytes of data, returns char * */
		char * File::readBin(unsigned int size, void * dest)
		{
			if (!size) size = this->size;
			if (!dest) dest = malloc(size);
			p_istream->read((char *) dest, size);
			return (char *) dest;
		}

		// close instream
		void File::close() { p_istream->close(); }

		// Returns position of end of archived file whose header was last digested
		unsigned long File::end() { return offset + size; }

		/* compute and set and return checksum */
		unsigned int Header::checksum()
		{
			unsigned int value = 0;
			char * header = (char *) this;
			// variables
			size_t i,
				size = sizeof(Header),
				checksum_s = offsetof(Header, _chksum),
				checksum_e = checksum_s + sizeof(Header::_chksum);
			// calculate checksum
			for (i = 0; i < size; i++)
				value += i >= checksum_s && i < checksum_e ? ' ' : header[i];
			// set checksum
			snprintf(_chksum, sizeof(_chksum), "%07o", value);
			return value;
		}

		wstring(name, _name)
		rstring(name, _name)

		wint(mode, _mode)
		rint(mode, _mode)
		rstring(modeStr, _mode)

		wint(uid, _uid)
		rint(uid, _uid)

		wint(gid, _gid)
		rint(gid, _gid)

		wlong(size, _size)
		rlong(size, _size)

		wlong(mtime, _mtime)
		rlong(mtime, _mtime)

		void Header::typeflag(char value) { _typeflag = value; }
		char Header::typeflag() { return _typeflag; }

		wstring(linkname, _linkname)
		rstring(linkname, _linkname)

		rstring(magic, _magic)

		string Header::version() { memset(_version, '0', 2); return string(_version, 0, sizeof _version); }

		wstring(uname, _uname)
		rstring(uname, _uname)

		wstring(gname, _gname)
		rstring(gname, _gname)

		rstring(devmajor, _devmajor)

		rstring(devminor, _devminor)

		rstring(prefix, _prefix)

		unsigned int Header::padding() { return size() ? USTAR_HEADER_SIZE - (size() % USTAR_HEADER_SIZE) : 0; }

		/* Constructors and constructor helpers */

		Header::Header() { zero(); setCommonValues(); }

		Header::Header(string name, long size, char typeflag)
		{
			zero();
			this->name( name );
			this->size( size );
			this->typeflag( typeflag );
			setCommonValues();
		}

		Header::Header(istream & in) { char * p = (char *) this; in.read(p, sizeof(Header)); }

		void Header::zero() { memset(this, 0, sizeof(Header)); }

		/* Set all fields except name, size, typeflag, chksum, devmajor, devminor, prefix */
		void Header::setCommonValues()
		{
			mode( _typeflag == USTAR_REGULAR ? 0755 : 644 );
			uid( 0 );
			gid( 0 );
			uname( "root" );
			gname( "root" );
			memcpy( _magic, "ustar", sizeof(_magic) );
			memset( _version, '0', 2 );
			time_t timer;
			time( &timer );
			mtime( timer );
		}

		/* IO functions */

		void Header::toStdout(ostream & streamOut)
		{
			(streamOut)
				<< name() << endl
				<< modeStr() << endl
				<< uid() << endl
				<< gid() << endl
				<< size() << endl
				<< mtime() << endl
				<< checksum() << endl
				<< _typeflag << endl
				<< linkname() << endl
				<< magic() << endl
				<< version() << endl
				<< uname() << endl
				<< gname() << endl
				<< devmajor() << endl
				<< devminor() << endl
				<< prefix() << endl
				;
		}
		
		void Header::pad(ostream & streamOut)	{	int i; for (i=0; i < padding(); i++) { streamOut.put(0); } }
		
		void Header::out(ostream & streamOut)
		{
			assert( _name[0] && _mode[0] && _uid[0] && _gid[0] && _size[0] && _mtime[0] && _magic[0] && _version[0] && _uname[0] && _gname[0] );
			if (! _chksum[0]) { checksum(); }
			streamOut.write((char *) this, sizeof(Header));
		}

		Header * Header::in(istream & streamIn) { return new Header(streamIn); }

		void Header::skip(istream & streamIn) { streamIn.seekg(size(), ios::cur); }
	}
}

#undef rstring
#undef wstring
#undef rint
#undef wint
#undef rlong
#undef wlong
