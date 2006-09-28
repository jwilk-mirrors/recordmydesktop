/*********************************************************************************
*                             recordMyDesktop                                    *
**********************************************************************************
*                                                                                *
*             Copyright (C) 2006  John Varouhakis                                *
*                                                                                *
*                                                                                *
*    This program is free software; you can redistribute it and/or modify        *
*    it under the terms of the GNU General Public License as published by        *
*    the Free Software Foundation; either version 2 of the License, or           *
*    (at your option) any later version.                                         *
*                                                                                *
*    This program is distributed in the hope that it will be useful,             *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*    GNU General Public License for more details.                                *
*                                                                                *
*    You should have received a copy of the GNU General Public License           *
*    along with this program; if not, write to the Free Software                 *
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   *
*                                                                                *
*                                                                                *
*                                                                                *
*    For further information contact me at johnvarouhakis@gmail.com              *
**********************************************************************************/

#include <recordmydesktop.h>

void *FlushToOgg(void *pdata){
    int videoflag=0,audioflag=0;
    double video_bytesout=0,audio_bytesout=0;
    ogg_page videopage,audiopage;
    while(((ProgData *)pdata)->running){
        videoflag=ogg_stream_pageout(&((ProgData *)pdata)->enc_data->m_ogg_ts,&videopage);
        if(videoflag){
            video_bytesout+=fwrite(videopage.header,1,videopage.header_len,((ProgData *)pdata)->enc_data->fp);
            video_bytesout+=fwrite(videopage.body,1,videopage.body_len,((ProgData *)pdata)->enc_data->fp);
            videoflag=0;
            if(!((ProgData *)pdata)->args.nosound){
                audioflag=ogg_stream_pageout(&((ProgData *)pdata)->enc_data->m_ogg_vs,&audiopage);
                if(audioflag){
                    audio_bytesout+=fwrite(audiopage.header,1,audiopage.header_len,((ProgData *)pdata)->enc_data->fp);
                    audio_bytesout+=fwrite(audiopage.body,1,audiopage.body_len,((ProgData *)pdata)->enc_data->fp);
                    audioflag=0;
                }
            }
        }
        else
            usleep(10000);
      
    }
    //last packages
    videoflag=ogg_stream_flush(&((ProgData *)pdata)->enc_data->m_ogg_ts,&videopage);
    if(videoflag){
        video_bytesout+=fwrite(videopage.header,1,videopage.header_len,((ProgData *)pdata)->enc_data->fp);
        video_bytesout+=fwrite(videopage.body,1,videopage.body_len,((ProgData *)pdata)->enc_data->fp);
        videoflag=0;
    }
    if(!((ProgData *)pdata)->args.nosound)
        audioflag=ogg_stream_flush(&((ProgData *)pdata)->enc_data->m_ogg_vs,&audiopage);
    if(audioflag){
        audio_bytesout+=fwrite(audiopage.header,1,audiopage.header_len,((ProgData *)pdata)->enc_data->fp);
        audio_bytesout+=fwrite(audiopage.body,1,audiopage.body_len,((ProgData *)pdata)->enc_data->fp);
        audioflag=0;
    }

    ogg_stream_clear(&((ProgData *)pdata)->enc_data->m_ogg_ts);
    if(!((ProgData *)pdata)->args.nosound)
        ogg_stream_clear(&((ProgData *)pdata)->enc_data->m_ogg_vs);
//this always gives me a segfault :(
//     theora_clear(&((ProgData *)pdata)->enc_data->m_th_st);
    
    if(((ProgData *)pdata)->enc_data->fp)fclose(((ProgData *)pdata)->enc_data->fp);

    fprintf(stderr,"\r   \nDone.\nWritten %.0f bytes\n(%.0f of which were video data and %.0f audio data)\n\n",video_bytesout+audio_bytesout,video_bytesout,audio_bytesout);
    pthread_exit(&errno);
}
