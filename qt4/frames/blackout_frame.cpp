#include "blackout_frame.hpp"
#include "gui_helpers.hpp"
#include "filter_win.hpp"
#include "thumbnail.hpp"
#include "../main.hpp"
#include "../../filter.hpp"
#include <vlc_configuration.h>
#include <vlc_input.h>
#include <vlc_vout.h>
#include "../../../../src/video_output/snapshot.h"
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <cstdio>
#include <vlc_fs.h>

// test
#include <QBitmap>
#include <cstring>
#include <iostream>
using namespace std;

#define MOVIESOAP_DUMP_COORD_TEXT(name) (mod->mod.name = name##Text->text().toInt())
#define MOVIESOAP_LOAD_COORD_TEXT(name) (name##Text->setText(QString::number(mod->mod.name)))
#define MOVIESOAP_LOAD_COORD_TEXT_FINT(name) (name##Text->setText(QString::number(name)))
#define MOVIESOAP_SNAPSHOT_FNAME "moviesoap-snapshot.bmp"

namespace Moviesoap
{
	/* Given a block containing an image, write image to filesystem */
	inline int writeSnapshotToFile(block_t *p_image)
	{
		FILE *file = vlc_fopen(MOVIESOAP_SNAPSHOT_FNAME, "wb");
		int success = MOVIESOAP_SUCCESS;
		if (!file)
			return MOVIESOAP_ENOFILE;
		else if (fwrite(p_image->p_buffer, p_image->i_buffer, 1, file) != 1)
			success = MOVIESOAP_EFILEIO;
		else
			return MOVIESOAP_SUCCESS;
		fclose( file );
		return success;
	}

	/* Conveneience method */
	inline uint32_t bytes_to_int(char * data, int length)
	{
		uint32_t output = 0;
		for (int i = length; i > 0; i--)
			output = (output <<  8) + data[i-1];
		return output;
	}

	/* Returns block to bmp image (or null). Requires block_Release() */
	block_t * BlackoutFrame::takeSnapshot(mtime_t usec)
	{
		// seek time
		var_SetTime( Moviesoap::p_input, "time", usec );
		// get vout thread
		vout_thread_t * p_vout = input_GetVout( Moviesoap::p_input );
		if ( !p_vout ) { return NULL; }
		// take snapshot
		picture_t *p_picture = NULL;
    	block_t *p_image = NULL;
    	video_format_t fmt;
    	if ( vout_GetSnapshot( p_vout, &p_image, &p_picture, &fmt, "bmp", 500*1000 ) ) {
    		// handle failure in vout_GetSnapshot()
    		if (p_image) {
    			free(p_image);
    			p_image = NULL;
    		}
    	}
    	// cleanup
    	if (p_vout)
			vlc_object_release( p_vout );
		if (p_picture)
			picture_Release( p_picture );
		// return
		return p_image;
	}

	/* Takes .bmp file and loads it into thumbnail QLabel */
	void BlackoutFrame::loadImageFromFile() {
		QPixmap pixmap = QPixmap(QString(MOVIESOAP_SNAPSHOT_FNAME), "bmp");
		thumbnail->setPixmap(pixmap);
	}


	/* If vout thread is available. Move to start_time of mod, take snapshot, load snapshot. */
	void BlackoutFrame::captureAndLoadImage(Mod * mod)
	{
		// check for input
		if ( !Moviesoap::p_input ) { return; }
		// take snapshot
		block_t * p_image = takeSnapshot(mod->mod.start); // todo should be mod->mod.start
		if ( p_image ) {
			// write snapshot to file
			if ( writeSnapshotToFile( p_image ) == MOVIESOAP_SUCCESS ) {
				// load the image to the GUI
				loadImageFromFile();
			}
			// cleanup
			block_Release( p_image );
		}
	}

	/* Cb: OK button clicked. Dump fields to Mod. Change frame to modFrame */
	void BlackoutFrame::okClicked()
	{
		// update p_editingMod
		dump(filterWin->p_mod);
		// show mod pane
		filterWin->toModFrame();
	}

	/* Set GUI inputs according to args */
	void BlackoutFrame::load(int x1, int y1, int x2, int y2)
	{
		MOVIESOAP_LOAD_COORD_TEXT_FINT(x1);
		MOVIESOAP_LOAD_COORD_TEXT_FINT(x2);
		MOVIESOAP_LOAD_COORD_TEXT_FINT(y1);
		MOVIESOAP_LOAD_COORD_TEXT_FINT(y2);
	}

	/* Set GUI inputs according to Mod fields */
	void BlackoutFrame::load(Mod * mod)
	{
		MOVIESOAP_LOAD_COORD_TEXT(x1);
		MOVIESOAP_LOAD_COORD_TEXT(x2);
		MOVIESOAP_LOAD_COORD_TEXT(y1);
		MOVIESOAP_LOAD_COORD_TEXT(y2);
		captureAndLoadImage(mod);
	}

	/* Set Mod fields according to GUI inputs */
	void BlackoutFrame::dump(Mod * mod)
	{
		MOVIESOAP_DUMP_COORD_TEXT(x1);
		MOVIESOAP_DUMP_COORD_TEXT(x2);
		MOVIESOAP_DUMP_COORD_TEXT(y1);
		MOVIESOAP_DUMP_COORD_TEXT(y2);
	}

	/* Not working */
	void BlackoutFrame::loadImageFromBlock(block_t *p_block) {
		char * data = (char *) p_block->p_buffer;
		bmp_file_header_t * file_header = (bmp_file_header_t *) data;
		
		uint32_t offset = bytes_to_int(file_header->bf_off_bits, 4);
		cout << "offset :" << dec << offset << endl;
		bmp_img_header_t * img_header = (bmp_img_header_t *) data + offset;

		cout << "header size: " << endl
		<< dec << bytes_to_int(img_header->size, 4) << endl
		<< dec << (int) img_header->size << endl
		<< dec << (uint32_t) img_header->size << endl;
		
		// char * bits = data + offset;
		// QBitmap bitmap = QBitmap::fromData(QSize(), bits);
		// thumbnail->setPixmap(bitmap);
	}
	
	/* Constructor */
	BlackoutFrame::BlackoutFrame(FilterWin *parent) : QFrame(parent), filterWin(parent)
	{
		QVBoxLayout *layout = new QVBoxLayout;
		setLayout(layout);
		// coord div
		QGridLayout *grid = new QGridLayout;
		y1Text = addTextToGrid(grid, 0, 1, "top");
		x1Text = addTextToGrid(grid, 1, 0, "left");
		x2Text = addTextToGrid(grid, 1, 2, "right");
		y2Text = addTextToGrid(grid, 2, 1, "bottom");
		layout->addLayout(grid);
		// button div
		QPushButton *okButton = new QPushButton(QString("&Ok"));
		connect( okButton, SIGNAL(clicked()), this, SLOT(okClicked()) );
		layout->addWidget(okButton);
		// thumbnail div
		thumbnail = new Thumbnail(tr("If you have a video open,\na blackout preview will\nappear in this box."));
		thumbnail->setFixedSize(400,350);
		connect( thumbnail, SIGNAL(blackoutChanged(int,int,int,int)), this, SLOT(load(int,int,int,int)) );
		layout->addWidget(thumbnail);
	}
}

#undef MOVIESOAP_LOAD_COORD_TEXT
#undef MOVIESOAP_DUMP_COORD_TEXT