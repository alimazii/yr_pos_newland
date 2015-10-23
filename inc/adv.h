#ifndef _ADV_H_
#define _ADV_H_

enum img_post_type {
	  IMG_CLEAR = 0,
	  IMG_CONFIRM,
	  IMG_DEL_CONFIRM
};

#ifdef __cplusplus
extern "C" 
{
#endif

int initLoadImg(char* ImgName);
int imgOpPost(int type, char* post_data);	

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif