
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_Component_curve_H__
#define __INC_Component_curve_H__

#include <stdint.h>
#include <string.h>
#include "lw_oopc.h"

#include "sdhDef.h"
#include "dev_lcd.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define CRV_MAX_PIXS			240  //曲线最大的像素点数量


#define CRV_CTL_HIDE					0
#define CRV_CTL_STEP_PIX				1
#define CRV_CTL_MAX_NUM					2

//曲线显示控制
#define	CRV_SHOW_WHOLE				0		//整根曲线都显示
#define	CRV_SHOW_LATEST				1		//只显示最新添加的点

#define FLEX_ZOOM_IN				0		//放大
#define FLEX_ZOOM_OUT				1		//缩小
#define FLEX_CLEAN					2		//清除
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------


typedef struct
{
	uint8_t					crv_col;
	uint8_t					crv_direction;
	uint8_t					crv_step_pix;		//
	uint8_t					crv_flag;
	uint16_t				crv_x0,crv_y0;
	uint16_t				crv_x1,crv_y1;
	
	uint16_t				crv_buf_size;
	uint16_t				crv_max_num_data;
}curve_att_t;

typedef struct {
	int16_t		up_limit;
	int16_t		lower_limit;
	int16_t		val;
	uint16_t		prc;
}crv_val_t;
typedef struct {
	uint8_t			*p_vals_y;
	uint16_t		crv_num_points;		//已有的点数
	uint16_t		next_index;
	//当前曲线的起始索引，当曲线满了以后不清除的情况下，其实索引才会发生变化
	//对于清除曲线的情况下，curve_start_index始终为0
	uint16_t		crv_start_index;

	uint8_t			none[2];
}crv_run_info_t;

typedef struct {
	uint8_t				crv_bkg_dirty;
	uint8_t				crv_bkg_id;		//如果背景的区域不一样，那么这个值就不一样
	uint8_t				none[2];
}crv_back_ground_info_t;

CLASS(Curve)
{
	uint8_t		set_free_curve;
	uint8_t		set_vaild_curve;
	uint8_t		num_curve;
	uint8_t		total_pix;

	curve_att_t							*p_crv_att;
	crv_run_info_t					*p_run_info;
	crv_back_ground_info_t	*p_bkg_info;
	void 		(*init)(Curve *self, uint8_t	num_curve);
	int			(*alloc)(curve_att_t  *c);
	void		(*free)(uint8_t  crv_fd);
	void		(*reset)(uint8_t  crv_fd);
	void		(*add_point)(uint8_t  crv_fd, uint8_t prc);
	void		(*crv_ctl)(uint8_t  crv_fd, uint8_t	ctl, uint16_t val);
	void		(*crv_show_bkg)(void);
	void		(*crv_show_curve)(uint8_t  crv_fd, uint8_t show_ctl);
	void		(*crv_data_flex)(uint8_t	crv_fd, char flex, uint8_t	scale, uint16_t new_max_num);
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Curve	*CRV_Get_Sington(void);


#endif

