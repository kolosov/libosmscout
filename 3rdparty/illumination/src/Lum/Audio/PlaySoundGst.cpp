/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <Lum/Audio/PlaySoundGst.h>

#include <Lum/Base/String.h>

#include <Lum/OS/EventLoopGMainLoop.h>

#include <Lum/OS/X11/Display.h>

#include <Lum/Private/Config.h>

#include <iostream>
namespace Lum {
  namespace Audio {

    gboolean GstCallback(GstBus* bus, GstMessage* message, gpointer data)
    {
      PlaySoundGst* playSound=(PlaySoundGst*)data;

      if (GST_MESSAGE_TYPE(message)==GST_MESSAGE_STATE_CHANGED) {
        GstState oldState,newState,pendingState;

        gst_message_parse_state_changed(message,&oldState,&newState,&pendingState);

        //std::cout << "State change from " << gst_element_state_get_name(oldState) << " to " << gst_element_state_get_name(newState) << " pending " << gst_element_state_get_name(pendingState) << std::endl;
        playSound->state=newState;
      }
      else if (GST_MESSAGE_TYPE(message)==GST_MESSAGE_TAG) {
        GstTagList *tagList;
        char       *value;

        gst_message_parse_tag(message,&tagList);

        value=NULL;
        gst_tag_list_get_string(tagList,GST_TAG_TITLE,&value);
        if (value!=NULL) {
          //std::cout << "Title: " << value << std::endl;
          g_free(value);
        }
        value=NULL;
        gst_tag_list_get_string(tagList,GST_TAG_ARTIST,&value);
        if (value!=NULL) {
          //std::cout << "Artist: " << value << std::endl;
          g_free(value);
        }
        value=NULL;
        gst_tag_list_get_string(tagList,GST_TAG_ALBUM,&value);
        if (value!=NULL) {
          //std::cout << "Album: " << value << std::endl;
          g_free(value);
        }
        /*gst_tag_list_get_string(tagList,GST_TAG_DATE,&value);
        std::cout << "Date: " << value << std::endl;
        g_free(value);*/
        value=NULL;
        gst_tag_list_get_string(tagList,GST_TAG_GENRE,&value);
        if (value!=NULL) {
          //std::cout << "Genre: " << value << std::endl;
          g_free(value);
        }
        value=NULL;
        gst_tag_list_get_string(tagList,GST_TAG_COMMENT,&value);
        if (value!=NULL) {
          //std::cout << "Comment: " << value << std::endl;
          g_free(value);
        }
        value=NULL;
        gst_tag_list_get_string(tagList,GST_TAG_LOCATION,&value);
        if (value!=NULL) {
          //std::cout << "Location: " << value << std::endl;
          g_free(value);
        }

        gst_tag_list_free(tagList);
      }
      else if (GST_MESSAGE_TYPE(message)==GST_MESSAGE_EOS) {
        //std::cout << "Message: EOS" << std::endl;
      }
      else {
        //std::cout << "Message: " << gst_message_type_get_name(GST_MESSAGE_TYPE(message)) << std::endl;
      }

      return false;
    }

    PlaySoundGst::PlaySoundGst()
    : bin(NULL),
      gsource(NULL),
      state(GST_STATE_NULL)
    {
    }

    PlaySoundGst::~PlaySoundGst()
    {
      if (bin!=NULL) {
        gst_element_set_state(bin,GST_STATE_NULL);
        ::gst_object_unref(GST_OBJECT(bin));
      }

      if (gsource!=NULL) {
        g_source_destroy(gsource);
        g_source_unref(gsource);
      }
    }

    PlaySound::State PlaySoundGst::GetState() const
    {
      switch(state) {
      case GST_STATE_VOID_PENDING:
      case GST_STATE_NULL:
        return stateNull;
      case GST_STATE_READY:
        return stateReady;
      case GST_STATE_PAUSED:
        return statePaused;
      case GST_STATE_PLAYING:
        return statePlaying;
      }

      return stateNull;
    }

    void PlaySoundGst::SetFilename(const std::wstring& filename, Type type)
    {
      this->filename=filename;
      this->type=type;
    }

    void PlaySoundGst::Play()
    {
      if (bin==NULL) {
        bin=::gst_element_factory_make("playbin","playbin");

        if (bin!=NULL) {
          gchar *uri=::g_filename_to_uri(Lum::Base::WStringToString(filename).c_str(),NULL,NULL);

          assert(uri!=NULL);

          ::g_object_set(G_OBJECT(bin),"uri",uri,NULL);
        }
        else if (type==typeWav) {
          ::GError     *error=NULL;
          ::GstElement *pipeline,*source;

#if defined(LUM_HAVE_LIB_HILDON)
          pipeline=gst_parse_launch("filesrc name=source ! wavparse ! dsppcmsink",&error);
#else
          pipeline=gst_parse_launch("filesrc name=source ! wavparse ! autoaudiosink",&error);
#endif
          if (pipeline==NULL) {
            std::cerr << "Cannot create pipeline!" << std::endl;
            return;
          }

          source=gst_bin_get_by_name(GST_BIN(pipeline),"source");
          ::g_object_set(G_OBJECT(source),"location",Lum::Base::WStringToString(filename).c_str(),NULL);

          bin=pipeline;
        }
        else {
          std::cerr << " No way to play file!" << std::endl;
          return;
        }

        if (bin!=NULL) {
          OS::GMainLoopEventLoop* eventLoop=dynamic_cast<OS::GMainLoopEventLoop*>(OS::display->GetEventLoop());

          gsource=::gst_bus_create_watch(gst_pipeline_get_bus(GST_PIPELINE(bin)));
          ::g_source_set_callback(gsource,(GSourceFunc)GstCallback,this,NULL);
          g_source_attach(gsource,eventLoop->context);
        }
      }

      if (bin!=NULL) {
        ::gst_element_set_state(bin,GST_STATE_PAUSED);
        if (!::gst_element_seek(bin,
                                1.0,
                                GST_FORMAT_TIME,
                                GST_SEEK_FLAG_FLUSH,
                                GST_SEEK_TYPE_SET,0,
                                GST_SEEK_TYPE_NONE,-1)) {
          std::cout << "Cannot seek back!" << std::endl;
        }

        ::gst_element_set_state(bin,GST_STATE_PLAYING);
      }
    }

    void PlaySoundGst::Pause()
    {
      if (bin!=NULL) {
        ::gst_element_set_state(bin,GST_STATE_PAUSED);
      }
    }

    void PlaySoundGst::Stop()
    {
      if (bin!=NULL) {
        ::gst_element_set_state(bin,GST_STATE_NULL);
      }
    }
  }
}
