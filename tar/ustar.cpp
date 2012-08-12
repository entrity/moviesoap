<<<<<<< HEAD
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
=======
#include "ustar.h"
#include <limits.h>
#include "packed.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

typedef unsigned char uint8_t;

/* Header for ustar-format tar archive.  See the documentation of
   the "pax" utility in [SUSv3] for the the "ustar" format
   specification. */
struct ustar_header
  {
    char name[100];             /* File name.  Null-terminated if room. */
    char mode[8];               /* Permissions as octal string. */
    char uid[8];                /* User ID as octal string. */
    char gid[8];                /* Group ID as octal string. */
    char size[12];              /* File size in bytes as octal string. */
    char mtime[12];             /* Modification time in seconds
                                   from Jan 1, 1970, as octal string. */
    char chksum[8];             /* Sum of octets in header as octal string. */
    char typeflag;              /* An enum ustar_type value. */
    char linkname[100];         /* Name of link target.
                                   Null-terminated if room. */
    char magic[6];              /* "ustar\0" */
    char version[2];            /* "00" */
    char uname[32];             /* User name, always null-terminated. */
    char gname[32];             /* Group name, always null-terminated. */
    char devmajor[8];           /* Device major number as octal string. */
    char devminor[8];           /* Device minor number as octal string. */
    char prefix[155];           /* Prefix to file name.
                                   Null-terminated if room. */
    char padding[12];           /* Pad to 512 bytes. */
  } PACKED;

/* Returns the checksum for the given ustar format HEADER. */
static unsigned int
calculate_chksum (const struct ustar_header *h)
{
  const uint8_t *header = (const uint8_t *) h;
  unsigned int chksum;
  size_t i;

  chksum = 0;
  for (i = 0; i < USTAR_HEADER_SIZE; i++)
    {
      /* The ustar checksum is calculated as if the chksum field
         were all spaces. */
      const size_t chksum_start = offsetof(struct ustar_header, chksum);
      const size_t chksum_end = chksum_start + sizeof h->chksum;
      bool in_chksum_field = i >= chksum_start && i < chksum_end;
      chksum += in_chksum_field ? ' ' : header[i];
    }
  return chksum;
}

/* Drop possibly dangerous prefixes from FILE_NAME and return the
   stripped name.  An archive with file names that start with "/"
   or "../" could cause a naive tar extractor to write to
   arbitrary parts of the file system, not just the destination
   directory.  We don't want to create such archives or be such a
   naive extractor.

   The return value can be a suffix of FILE_NAME or a string
   literal. */
static const char *
strip_antisocial_prefixes (const char *file_name)
{
  while (*file_name == '/'
         || !memcmp (file_name, "./", 2)
         || !memcmp (file_name, "../", 3))
    file_name = strchr (file_name, '/') + 1;
  return *file_name == '\0' || !strcmp (file_name, "..") ? "." : file_name;
}

/* Composes HEADER as a USTAR_HEADER_SIZE (512)-byte archive
   header in ustar format for a SIZE-byte file named FILE_NAME of
   the given TYPE.  The caller is responsible for writing the
   header to a file or device.

   If successful, returns true.  On failure (due to an
   excessively long file name), returns false. */
bool
ustar_make_header (const char *file_name, enum ustar_type type,
                   int size, char header[USTAR_HEADER_SIZE])
{
  struct ustar_header *h = (struct ustar_header *) header;

  assert(sizeof (struct ustar_header) == USTAR_HEADER_SIZE);
  assert(type == USTAR_REGULAR || type == USTAR_DIRECTORY);

  /* Check file name. */
  file_name = strip_antisocial_prefixes (file_name);
  if (strlen (file_name) > 99)
    {
      printf ("%s: file name too long\n", file_name);
      return false;
    }

  /* Fill in header except for final checksum. */
  memset (h, 0, sizeof *h);
  memcpy (h->name, file_name, sizeof h->name);
  snprintf (h->mode, sizeof h->mode, "%07o",
            type == USTAR_REGULAR ? 0644 : 0755);
  memcpy (h->uid, "0000000", sizeof h->uid);
  memcpy (h->gid, "0000000", sizeof h->gid);
  snprintf (h->size, sizeof h->size, "%011o", size);
  snprintf (h->mtime, sizeof h->size, "%011o", 1136102400);
  h->typeflag = type;
  memcpy (h->magic, "ustar", sizeof h->magic);
  h->version[0] = h->version[1] = '0';
  memcpy (h->gname, "root", sizeof h->gname);
  memcpy (h->uname, "root", sizeof h->uname);

  /* Compute and fill in final checksum. */
  snprintf (h->chksum, sizeof h->chksum, "%07o", calculate_chksum (h));

  return true;
}

/* Parses a SIZE-byte octal field in S in the format used by
   ustar format.  If successful, stores the field's value in
   *VALUE and returns true; on failure, returns false.

   ustar octal fields consist of a sequence of octal digits
   terminated by a space or a null byte.  The ustar specification
   seems ambiguous as to whether these fields must be padded on
   the left with '0's, so we accept any field that fits in the
   available space, regardless of whether it fills the space. */
static bool
parse_octal_field (const char *s, size_t size, unsigned long int *value)
{
  size_t ofs;

  *value = 0;
  for (ofs = 0; ofs < size; ofs++)
    {
      char c = s[ofs];
      if (c >= '0' && c <= '7')
        {
          if (*value > ULONG_MAX / 8)
            {
              /* Overflow. */
              return false;
            }
          *value = c - '0' + *value * 8;
        }
      else if (c == ' ' || c == '\0')
        {
          /* End of field, but disallow completely empty
             fields. */
          return ofs > 0;
        }
      else
        {
          /* Bad character. */
          return false;
        }
    }

  /* Field did not end in space or null byte. */
  return false;
}

/* Returns true if the CNT bytes starting at BLOCK are all zero,
   false otherwise. */
static bool
is_all_zeros (const char *block, size_t cnt)
{
  while (cnt-- > 0)
    if (*block++ != 0)
      return false;
  return true;
}

/* Parses HEADER as a ustar-format archive header for a regular
   file or directory.  If successful, stores the archived file's
   name in *FILE_NAME (as a pointer into HEADER or a string
   literal), its type in *TYPE, and its size in bytes in *SIZE,
   and returns a null pointer.  On failure, returns a
   human-readable error message. */
const char *
ustar_parse_header (const char header[USTAR_HEADER_SIZE],
                    const char **file_name, enum ustar_type *type, int *size)
{
  const struct ustar_header *h = (const struct ustar_header *) header;
  unsigned long int chksum, size_ul;

  assert(sizeof (struct ustar_header) == USTAR_HEADER_SIZE);

  /* Detect end of archive. */
  if (is_all_zeros (header, USTAR_HEADER_SIZE))
    {
      *file_name = NULL;
      *type = USTAR_EOF;
      *size = 0;
      return NULL;
    }

  /* Validate ustar header. */
  if (memcmp (h->magic, "ustar", 6))
    return "not a ustar archive";
  else if (h->version[0] != '0' || h->version[1] != '0')
    return "invalid ustar version";
  else if (!parse_octal_field (h->chksum, sizeof h->chksum, &chksum))
    return "corrupt chksum field";
  else if (chksum != calculate_chksum (h))
    return "checksum mismatch";
  else if (h->name[sizeof h->name - 1] != '\0' || h->prefix[0] != '\0')
    return "file name too long";
  else if (h->typeflag != USTAR_REGULAR && h->typeflag != USTAR_DIRECTORY)
    return "unimplemented file type";
  if (h->typeflag == USTAR_REGULAR)
    {
      if (!parse_octal_field (h->size, sizeof h->size, &size_ul))
        return "corrupt file size field";
      else if (size_ul > INT_MAX)
        return "file too large";
    }
  else
    size_ul = 0;

  /* Success. */
  *file_name = strip_antisocial_prefixes (h->name);
  *type = (ustar_type) h->typeflag;
  *size = size_ul;
  return NULL;
}

>>>>>>> 89a430f389d595358d1859b4f1dd357820c7c998
