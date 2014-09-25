#include "JPEGFile.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>

using namespace std;

#define SLASH '\\'
#define MAX_COMMENT_SIZE 2000
#define MAX_DATE_COPIES 10

/*
* JPEG markers consist of one or more 0xFF bytes, followed by a marker
* code byte (which is not an FF).  Here are the marker codes of interest
* in this program.  (See jdmarker.c for a more complete list.)
*/

#define M_SOF0  0xC0		/* Start Of Frame N */
#define M_SOF1  0xC1		/* N indicates which compression process */
#define M_SOF2  0xC2		/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5		/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8		/* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9		/* End Of Image (end of datastream) */
#define M_SOS   0xDA		/* Start Of Scan (begins compressed data) */
#define M_APP0	0xE0		/* Application-specific marker, type N */
#define M_APP12	0xEC		/* (we don't bother to list all 16 APPn's) */
#define M_COM   0xFE		/* COMment */

CJPEGFile::CJPEGFile(lua_State* L)
{
	m_luaState = L;
}


CJPEGFile::~CJPEGFile()
{
}

void CJPEGFile::refreshAsset()
{
	// invalidate the current JPEG file and initialize the new file.
}

SImageProperties CJPEGFile::getImageSize()
{
	return m_properties;
}

bool CJPEGFile::openFileStream(string& fileName)
{
	m_filePointer = fopen(fileName.c_str(), "rb");
	if (m_filePointer != nullptr)
		return true;
	return false;
}

bool CJPEGFile::loadAsset(string& fileName)
{
	openFileStream(fileName);
	scan_JPEG_header(0, 0);
	return false;
}

// The following processing code adapted from IJG's library
// http://http://www.ijg.org/

void CJPEGFile::ERREXIT(const char* msg)
{
	cout << msg << endl;
}

/* Read one byte, testing for EOF */
INT32 CJPEGFile::read_1_byte(void)
{
	INT32 c;

	c = getc(m_filePointer);
	if (c == EOF)
		ERREXIT("Premature EOF in JPEG file");
	return c;
}

/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
UINT32 CJPEGFile::read_2_bytes(void)
{
	INT32 c1, c2;

	c1 = getc(m_filePointer);
	if (c1 == EOF)
		ERREXIT("Premature EOF in JPEG file");
	c2 = getc(m_filePointer);
	if (c2 == EOF)
		ERREXIT("Premature EOF in JPEG file");
	return (((UINT32)c1) << 8) + ((UINT32)c2);
}

/*
* Find the next JPEG marker and return its marker code.
* We expect at least one FF byte, possibly more if the compressor used FFs
* to pad the file.
* There could also be non-FF garbage between markers.  The treatment of such
* garbage is unspecified; we choose to skip over it but emit a warning msg.
* NB: this routine must not be used after seeing SOS marker, since it will
* not deal correctly with FF/00 sequences in the compressed image data...
*/

INT32 CJPEGFile::next_marker(void)
{
	INT32 c;
	INT32 discarded_bytes = 0;

	/* Find 0xFF byte; count and skip any non-FFs. */
	c = read_1_byte();
	while (c != 0xFF) {
		discarded_bytes++;
		c = read_1_byte();
	}
	/* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
	* are legal as pad bytes, so don't count them in discarded_bytes.
	*/
	do {
		c = read_1_byte();
	} while (c == 0xFF);

	if (discarded_bytes != 0) {
		fprintf(stderr, "Warning: garbage data found in JPEG file\n");
	}

	return c;
}


/*
* Read the initial marker, which should be SOI.
* For a JFIF file, the first two bytes of the file should be literally
* 0xFF M_SOI.  To be more general, we could use next_marker, but if the
* input file weren't actually JPEG at all, next_marker might read the whole
* file and then return a misleading error message...
*/

INT32 CJPEGFile::first_marker(void)
{
	INT32 c1, c2;

	c1 = getc(m_filePointer);
	c2 = getc(m_filePointer);
	if (c1 != 0xFF || c2 != M_SOI)
		ERREXIT("Not a JPEG file");
	return c2;
}


/*
* Most types of marker are followed by a variable-length parameter segment.
* This routine skips over the parameters for any marker we don't otherwise
* want to process.
* Note that we MUST skip the parameter segment explicitly in order not to
* be fooled by 0xFF bytes that might appear within the parameter segment;
* such bytes do NOT introduce new markers.
*/

void CJPEGFile::skip_variable(void)
/* Skip over an unknown or uninteresting variable-length marker */
{
	UINT32 length;

	/* Get the marker parameter length count */
	length = read_2_bytes();
	/* Length includes itself, so must be at least 2 */
	if (length < 2)
		ERREXIT("Erroneous JPEG marker length");
	length -= 2;
	/* Skip over the remaining bytes */
	while (length > 0) {
		(void)read_1_byte();
		length--;
	}
}


/*
* Process a COM marker.
* We want to print out the marker contents as legible text;
* we must guard against non-text junk and varying newline representations.
*/

void CJPEGFile::process_COM(INT32 raw)
{
	UINT32 length;
	INT32 ch;
	INT32 lastch = 0;

	/* Bill Allombert: set locale properly for isprint */
#ifdef HAVE_LOCALE_H
	setlocale(LC_CTYPE, "");
#endif

	/* Get the marker parameter length count */
	length = read_2_bytes();
	/* Length includes itself, so must be at least 2 */
	if (length < 2)
		ERREXIT("Erroneous JPEG marker length");
	length -= 2;

	while (length > 0) {
		ch = read_1_byte();
		if (raw) {
			putc(ch, stdout);
			/* Emit the character in a readable form.
			* Nonprintables are converted to \nnn form,
			* while \ is converted to \\.
			* Newlines in CR, CR/LF, or LF form will be printed as one newline.
			*/
		}
		else if (ch == '\r') {
			printf("\n");
		}
		else if (ch == '\n') {
			if (lastch != '\r')
				printf("\n");
		}
		else if (ch == '\\') {
			printf("\\\\");
		}
		else if (isprint(ch)) {
			putc(ch, stdout);
		}
		else {
			printf("\\%03o", ch);
		}
		lastch = ch;
		length--;
	}
	printf("\n");

	/* Bill Allombert: revert to C locale */
#ifdef HAVE_LOCALE_H
	setlocale(LC_CTYPE, "C");
#endif
}


/*
* Process a SOFn marker.
* This code is only needed if you want to know the image dimensions...
*/

void CJPEGFile::process_SOFn(INT32 marker)
{
	UINT32 length;
	UINT32 image_height, image_width;
	INT32 data_precision, num_components;
	const char * process;
	INT32 ci;

	length = read_2_bytes();	/* usual parameter length count */

	data_precision = read_1_byte();
	image_height = read_2_bytes();
	image_width = read_2_bytes();
	num_components = read_1_byte();

	switch (marker) {
	case M_SOF0:	process = "Baseline";  break;
	case M_SOF1:	process = "Extended sequential";  break;
	case M_SOF2:	process = "Progressive";  break;
	case M_SOF3:	process = "Lossless";  break;
	case M_SOF5:	process = "Differential sequential";  break;
	case M_SOF6:	process = "Differential progressive";  break;
	case M_SOF7:	process = "Differential lossless";  break;
	case M_SOF9:	process = "Extended sequential, arithmetic coding";  break;
	case M_SOF10:	process = "Progressive, arithmetic coding";  break;
	case M_SOF11:	process = "Lossless, arithmetic coding";  break;
	case M_SOF13:	process = "Differential sequential, arithmetic coding";  break;
	case M_SOF14:	process = "Differential progressive, arithmetic coding"; break;
	case M_SOF15:	process = "Differential lossless, arithmetic coding";  break;
	default:	process = "Unknown";  break;
	}

	printf("JPEG image is %uw * %uh, %d color components, %d bits per sample\n",
		image_width, image_height, num_components, data_precision);
	// scrape image dimensions for our utility's use
	m_properties.m_width = image_width;
	m_properties.m_height = image_height;
	m_properties.m_size = 0;

	printf("JPEG process: %s\n", process);

	if (length != (UINT32)(8 + num_components * 3))
		ERREXIT("Bogus SOF marker length");

	for (ci = 0; ci < num_components; ci++) {
		(void)read_1_byte();	/* Component ID code */
		(void)read_1_byte();	/* H, V sampling factors */
		(void)read_1_byte();	/* Quantization table number */
	}
}


/*
* Parse the marker stream until SOS or EOI is seen;
* display any COM markers.
* While the companion program wrjpgcom will always insert COM markers before
* SOFn, other implementations might not, so we scan to SOS before stopping.
* If we were only interested in the image dimensions, we would stop at SOFn.
* (Conversely, if we only cared about COM markers, there would be no need
* for special code to handle SOFn; we could treat it like other markers.)
*/

INT32 CJPEGFile::scan_JPEG_header(INT32 verbose, INT32 raw)
{
	INT32 marker;

	/* Expect SOI at start of file */
	if (first_marker() != M_SOI)
		ERREXIT("Expected SOI marker first");

	/* Scan miscellaneous markers until we reach SOS. */
	for (;;) {
		marker = next_marker();
		switch (marker) {
			/* Note that marker codes 0xC4, 0xC8, 0xCC are not, and must not be,
			* treated as SOFn.  C4 in particular is actually DHT.
			*/
		case M_SOF0:		/* Baseline */
		case M_SOF1:		/* Extended sequential, Huffman */
		case M_SOF2:		/* Progressive, Huffman */
		case M_SOF3:		/* Lossless, Huffman */
		case M_SOF5:		/* Differential sequential, Huffman */
		case M_SOF6:		/* Differential progressive, Huffman */
		case M_SOF7:		/* Differential lossless, Huffman */
		case M_SOF9:		/* Extended sequential, arithmetic */
		case M_SOF10:		/* Progressive, arithmetic */
		case M_SOF11:		/* Lossless, arithmetic */
		case M_SOF13:		/* Differential sequential, arithmetic */
		case M_SOF14:		/* Differential progressive, arithmetic */
		case M_SOF15:		/* Differential lossless, arithmetic */
			if (verbose)
				process_SOFn(marker);
			else
				skip_variable();
			break;

		case M_SOS:			/* stop before hitting compressed data */
			return marker;

		case M_EOI:			/* in case it's a tables-only JPEG stream */
			return marker;

		case M_COM:
			process_COM(raw);
			break;

		case M_APP12:
			/* Some digital camera makers put useful textual information into
			* APP12 markers, so we print those out too when in -verbose mode.
			*/
			if (verbose) {
				printf("APP12 contains:\n");
				process_COM(raw);
			}
			else
				skip_variable();
			break;

		default:			/* Anything else just gets skipped */
			skip_variable();		/* we assume it has a parameter count... */
			break;
		}
	} /* end loop */
}
