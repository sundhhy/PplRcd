#include "chnInfoPic.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "mem/CiiMem.h"

#include "model_conf.h"


//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static ro_char s_CIF_No_1[] = {"<text f=32 bkc=black clr=white>01</>"};
static ro_char s_CIF_unit_1[] = {"<text xali=r  f=32 bkc=gren clr=white>m3/h</>"};
static ro_char s_CIF_alarm_1[] = {"<text  f=32 yali=m bkc=red clr=white>HI</>"};
static ro_char s_CIF_data_1[] = {"<text xali=r yali=m f=64 bkc=gren clr=white>123456.0</>"};
static ro_char s_CIF_text_1[] = {"<text xali=m yali=r f=24 bkc=gren clr=white>第1通道</>"};
static ro_char s_CIF_box_1[] = {"<bu bx=300 by=180 cg=2 rg=4 bkc=gren xali=m clr=black></>"};


static ro_char s_CIF_No_2[] = {"<text f=24 bkc=black clr=white>01</>"};
static ro_char s_CIF_unit_2[] = {"<text xali=r  f=24 bkc=gren clr=white>m3/h</>"};
static ro_char s_CIF_alarm_2[] = {"<text  f=24 yali=m bkc=red clr=white>HI</>"};
static ro_char s_CIF_data_2[] = {"<text xali=r yali=m f=24 bkc=gren clr=white>123456.0</>"};
static ro_char s_CIF_text_2[] = {"<text xali=m yali=r f=24 bkc=gren clr=white>第1通道</>"};
static ro_char s_CIF_box_2[] = {"<bu bx=160 by=180 cg=2 rg=4 bkc=gren xali=m clr=black></>"};

static ro_char s_CIF_No_3[] = {"<text xali=l yali=l f=16 bkc=black clr=white>01</>"};
static ro_char s_CIF_unit_3[] = {"<text xali=r yali=l f=16 bkc=gren clr=black>m3/h</>"};
static ro_char s_CIF_alarm_3[] = {"<text xali=l yali=l f=16 bkc=red clr=black>HI</>"};
static ro_char s_CIF_data_3[] = {"<text xali=r  f=24 bkc=gren clr=black>123456.0</>"};
static ro_char s_CIF_text_3[] = {"<text xali=m yali=r f=16 bkc=gren clr=black>第1通道</>"};


static ro_char s_CIF_box_3[] = {"<bu bx=158 by=90 cg=2 rg=4 bkc=gren xali=l clr=black></>"};
static ro_char s_CIF_box_4[] = {"<bu bx=158 by=90 cg=2 rg=4 bkc=gren xali=m clr=black></>"};


static ro_char s_CIF_alarm_5_6[] = {"<text xali=l yali=l f=16 bkc=red clr=black>HI</>"};
static ro_char s_CIF_data_5_6[] = {"<text xali=r  f=16 bkc=gren clr=black>123456.0</>"};
static ro_char s_CIF_box_5[] = {"<bu bx=104 by=80 cg=2 rg=4 bkc=gren  clr=black></>"};
static ro_char s_CIF_box_6[] = {"<bu bx=104 by=84 cg=2 rg=4 bkc=gren xali=m clr=black></>"};


static ro_char s_chnNo[][2] = {"01", "02", "03", "04", "05", "06"};
static ro_char s_chnText[][7] = {"第1通道", "第2通道", "第3通道", "第4通道", "第5通道", "第6通道"};



//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	const char 	*p_No;
	const char	*p_unit;
	const char	*p_alarm;
	const char	*p_data;
	const char	*p_text;

	const char	*p_box;
}CIF_piccode_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static sheet * CIF_build( char num, CIF_piccode_t *p_code);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//




//void Bulid_ChnData(sheet **pp_shts, void *code, int ( *update)( void *p_sht, void *p_srcMdl))
//{
//	
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	Model			*p_mdl = NULL;
//	short 			i;
//	
//	
//	
//	p_shtctl = GetShtctl();

//	for( i = 0; i < BARHMI_NUM_BARS; i++) {
//		
//		
//		p_exp = ExpCreate("text");
//		pp_shts[i] = Sheet_alloc( p_shtctl);
//		p_exp->inptSht( p_exp, code, pp_shts[i]) ;
//		pp_shts[i]->cnt.colour = arr_clrs[i];
//		pp_shts[i]->id = i;
////		pp_shts[i]->cnt.mdl_aux = AUX_DATA;
//		
////		if(update) {
////			pp_shts[i]->update = update;
////			
////			
////			
////		}
//		
//		//todo: 改成通道
////		p_mdl = pp_shts[i]->p_mdl;
//		p_mdl->attach(p_mdl, (mdl_observer *)pp_shts[i]);
//	}
//}



sheet	*CIF_build_pic( char num, char total)
{
	sheet	*p_pic = NULL;
	CIF_piccode_t code;
	
	if( num >= total)
		goto exit;
	
	
	switch( total) 
	{
		case 1:
			code.p_No = s_CIF_No_1;
			code.p_unit = s_CIF_unit_1;
			code.p_alarm = s_CIF_alarm_1;
			code.p_data = s_CIF_data_1;
			code.p_box = s_CIF_box_1;
			code.p_text = s_CIF_text_1;
		
			break;
		case 2:
			code.p_No = s_CIF_No_2;
			code.p_unit = s_CIF_unit_2;
			code.p_alarm = s_CIF_alarm_2;
			code.p_data = s_CIF_data_2;
			code.p_box = s_CIF_box_2;
			code.p_text = s_CIF_text_2;

			break;
		
		case 3:
		case 4:
		case 5:
		case 6:	
			code.p_No = s_CIF_No_3;
			code.p_unit = s_CIF_unit_3;
			code.p_alarm = s_CIF_alarm_3;
			code.p_data = s_CIF_data_3;
			code.p_text = s_CIF_text_3;
		if( total == 3)
			code.p_box = s_CIF_box_3;
		else if( total == 4)
			code.p_box = s_CIF_box_4;
		else if( total == 5) {
			
			code.p_box = s_CIF_box_5;
			code.p_alarm = s_CIF_alarm_5_6;
			code.p_data = s_CIF_data_5_6;
		}
		else if( total == 6) {
			code.p_box = s_CIF_box_6;
			code.p_alarm = s_CIF_alarm_5_6;
			code.p_data = s_CIF_data_5_6;
			
		}
		
			break;
		default:
			goto exit;
			
		
	}
	p_pic = CIF_build( num, &code);
	exit:
	return p_pic;
	
}

sheet *Get_chnInfoPic_No( sheet *p_cinfPic)
{
	
	return p_cinfPic->pp_sub[0];
}
sheet *Get_chnInfoPic_alarm( sheet *p_cinfPic)
{
	
	return p_cinfPic->pp_sub[2];
}
sheet *Get_chnInfoPic_data( sheet *p_cinfPic)
{
	
	return p_cinfPic->pp_sub[3];
}
sheet *Get_chnInfoPic_text( sheet *p_cinfPic)
{
	
	return p_cinfPic->pp_sub[4];
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static sheet * CIF_build( char num, CIF_piccode_t *p_code)
{
	shtctl 		*p_shtctl = NULL;
	sheet 		*p_top;
	sheet 		*p_sub;
	Expr 		*p_exp ;
	const char	*p_no;
	sheet *		p_sht;
	char		*p[6];
	short		row = 0, col = 0;
	
	
	p_shtctl = GetShtctl();
	p_top = Sheet_alloc( p_shtctl);

	p_top->subAtt.numSubRow = 3;
	p_top->subAtt.numSubCol = 2;
	
	p_top->pp_sub = CALLOC( 3 * 2 , sizeof( sheet *));
	
	
	p[0] = (char *)p_code->p_No;
	p[1] = (char *)p_code->p_unit;
	p[2] = (char *)p_code->p_alarm;
	p[3] = (char *)p_code->p_data;
	p[4] = (char *)p_code->p_text;
	p[5] = NULL;
	
	p_exp = ExpCreate( "text");
	for( row = 0; row < 3; row++) {
		for( col = 0; col < 2; col ++) {
			if( p[ row * 2 + col] == NULL)
				continue;
			p_sub = Sheet_alloc( p_shtctl);
			
			p_exp->inptSht( p_exp, p[ row * 2 + col], p_sub) ;
			p_top->pp_sub[row * 2 + col] = p_sub;
		}
	}
	//更改通道号
	p_no = s_chnNo[ num];
	p_sht = Get_chnInfoPic_No( p_top);
	p_sht->cnt.data = (char *)p_no;
	
	p_no = s_chnText[ num];
	p_sht = Get_chnInfoPic_text( p_top);
	p_sht->cnt.data = (char *)p_no;
	
	p_sht = Get_chnInfoPic_alarm( p_top);
	p_sht->cnt.effects = GP_SET_EFF( p_sht->cnt.effects, EFF_HIDE);
	
	
	p_exp = ExpCreate( "box");
	p_exp->inptSht( p_exp, (void *)p_code->p_box, p_top) ;
	p_top->area.offset_x = 3;
	p_top->area.offset_y = 3;
	
	return p_top;

	
	
	
}










