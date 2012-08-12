#ifndef ENTRITY_USTAR_H
#define ENTRITY_USTAR_H

#ifndef USTAR_HEADER_SIZE
#define USTAR_HEADER_SIZE 512
#endif

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

namespace Entrity
{
	namespace Ustar
	{
		class File;
		class Header;

		enum {
		USTAR_REGULAR,
		USTAR_LINK,
		USTAR_SYMLINK,
		USTAR_CHAR_SPECIAL,
		USTAR_BLOCK_SPECIAL,
		USTAR_DIRECTORY,
		USTAR_FIFO_SPECIAL
		};

		class File
		{
		public:
			/* fields */
			unsigned long offset; // end of last header read
			ifstream * p_istream;
			unsigned long size; // size of data for latest-parsed Header
			int padding; // size of padding for latest-parsed Header (padding + size gets us to next)
			/* constructors */
			File(string filepath) : size(0), offset(0) { p_istream = new ifstream(filepath.c_str(), ios::in | ios::binary); }
			File(ifstream & in) : p_istream(&in), size(0), offset(0) {}
			~File()	{ if (p_istream) p_istream->close(); }
			/* functions */
			Header * readHeader();
			void next(); // move ahead in stream to end of data block for latest-read header
			char * readBin(unsigned int size = 0, void * dest = NULL);
			char * readText(unsigned int size = 0, void * dest = NULL);
			void close(); // close instream
			unsigned long end(); // gives position of end of archive file designated by latest header
		};

		/* Class describing header for file within archive */

		class Header
		{	
		public:	
			char _name[100]; /* File name. Null-terminated if room. */
			char _mode[8]; /* Permissions as octal string. */
			char _uid[8]; /* User ID as octal string. */
			char _gid[8]; /* Group ID as octal string. */

			char _size[12]; /* File size in bytes as octal string. */
			char _mtime[12]; /* Modification time in seconds from Jan 1, 1970, as octal string. */
			char _chksum[8]; /* Sum of octets in header as octal string. */

			char _typeflag; /* An enum ustar_type value. */
			char _linkname[100]; /* Name of link target. Null-terminated if room. */

			char _magic[6]; /* "ustar\0" */
			char _version[2]; /* "00" */
			char _uname[32]; /* User name, always null-terminated. */
			char _gname[32]; /* Group name, always null-terminated. */
			char _devmajor[8]; /* Device major number as octal string. */
			char _devminor[8]; /* Device minor number as octal string. */
			char _prefix[155]; /* Prefix to file name. Null-terminated if room. */
			char _padding[12]; /* Pad to 512 bytes. */

			/* Constructor. Zeroes out all fields. */
			Header();
			/* Constructor. Initializes all fields, incl checksum */
			Header(string name, long size=0, char typeflag=USTAR_REGULAR);
			/* Constructor. Reads block from stream */
			Header(istream & in);
			/* Mutator functions */
			void setCommonValues(); /* Set all fields except name, size, typeflag, chksum, devmajor, devminor, prefix */
			void name(string);
			void mode(unsigned int);
			void uid(unsigned int);
			void gid(unsigned int);	
			void size(unsigned int); // Set length of file contents. Returns amount of padding you should include at end of content, before next header. */
			void mtime(unsigned int);
			void typeflag(char);
			void linkname(string);
			void uname(string);
			void gname(string);
			void zero();
			/* Accessor functions */
			string name();
			unsigned int mode();
			string modeStr();
			unsigned int uid();
			unsigned int gid();
			unsigned long size();
			unsigned long mtime();
			char typeflag();
			unsigned int checksum();
			string linkname();
			string magic();
			string version();
			string uname();
			string gname();
			string devmajor();
			string devminor();
			string prefix();
			unsigned int padding();
			/* IO functions */
			void toStdout(ostream &out);
			void out(ostream &out); // Before this, set name and size; after this, write data & call pad(out)
			void pad(ostream &out);
			static Header * in(istream &in);
			void skip(istream &in);
		};
	}
}

#endif