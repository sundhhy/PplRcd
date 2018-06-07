#include "sheet.h"
#include "sdhDef.h"
#include "device.h"
#include "glyph.h"
#include "mem/CiiMem.h"

#include "sdhDef.h"
#ifdef NO_ASSERT
#include "basis/assert.h"
#else
#include "assert.h"
#endif


#include <string.h>


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
#define SHEET_USE		1

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static shtctl *p_signalShtctl = NULL;
const Except_T Sheet_Failed = { "Alloc Sheet Failed" };
static struct SHTCTL	sht_rsc;
//static shtDefCmd *signal_cmd;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Sheet_refreshsub( struct SHEET *p_sht);
//void ShtDefualtExc( shtCmd *self, struct SHEET *p_sht, void *arg);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
shtctl *GetShtctl(void)
{
	I_dev_lcd *lcd;
	uint16_t	vxsize, vysize;
	if( p_signalShtctl == NULL)
	{
		Dev_open(LCD_DEVID, (void *)&lcd);
//		lcd->open();
		lcd->getScrnSize( &vxsize, &vysize);
		p_signalShtctl = Shtctl_init( vxsize, vysize);
		
	}
	
	return p_signalShtctl;
	
}
struct SHTCTL* Shtctl_init(   uint16_t vxsize, uint16_t vysize)
{
	struct SHTCTL* p_ctl;
	int i;
    
//    p_ctl = CALLOC( 1, sizeof( struct SHTCTL));
    p_ctl = &sht_rsc;
    if( p_ctl == NULL)
        goto err;
    
   
    p_ctl->v.vxsize = vxsize;
    p_ctl->v.vysize = vysize;
    p_ctl->top = -1;        
    
    for( i = 0; i < MAX_SHEETS; i++)
    {
//		p_ctl->arr_sheets[i].update = ShtUpdate;
        p_ctl->arr_sheets[i].flags = 0;
//        p_ctl->arr_sheets[i].p_shtctl = p_ctl;
        
    }
    
err:	
	return p_ctl;
	
}
//int Sht_input( void *self, void *data, int len)
//{
//	return 0;
//	
//}

struct SHEET *Sheet_alloc( struct SHTCTL *p_ctl)
{
    struct SHEET *p_sht;
    int i;
    for( i = 0; i < MAX_SHEETS; i++)
    {
        if(  p_ctl->arr_sheets[i].flags == 0)
        {
            p_sht = p_ctl->arr_sheets + i;
            p_sht->flags = SHEET_USE;
            p_sht->height = -1; 
			p_sht->id = 0xff;
			p_sht->pp_sub = NULL;
//			p_sht->e_heifht = 0;
			
			p_sht->cnt.subType = 0;
			p_sht->cnt.effects = 0;
			
			p_sht->subAtt.numSubCol = 0;
			p_sht->subAtt.numSubRow = 0;
			p_sht->subAtt.subColGrap = 0;
			p_sht->subAtt.subRowGrap = 0;
//			p_sht->input = Sht_input;
			
//			p_sht->p_enterCmd = (shtCmd*)Get_shtDefCmd();
//			p_sht->p_enterCmd = NULL;
            return p_sht;
            
        }
        
    }
	
	
			
	Except_raise(&Sheet_Failed, __FILE__, __LINE__);
		
    
    return NULL;
    
    
}

void Sheet_setbuf( struct SHEET *p_sht, uint8_t *buf, int bxsize, int bysize, int col_inv)
{
//    p_sht->buf = buf;
//    p_sht->bxsize = bxsize;
//    p_sht->bysize = bysize;
//    p_sht->col_inv = col_inv;
//    return;
    
}

//int ShtUpdate( void *p_sht, void *p_mdl)
//{
//	struct SHEET *p = ( struct SHEET *)p_sht;
//	
//	if(Sheet_is_hide(p_sht))
//		return RET_OK;
//		
//	p->cnt.data = p->p_mdl->to_string( p->p_mdl, 0, NULL);
//	p->cnt.len = strlen( p->cnt.data);
//	Sheet_slide( p_sht);
//	return RET_OK;
//	
//}



//把高度设置在最高图层
void Sheet_append( struct SHEET *p_sht)
{
	
}

void Sheet_updown( struct SHEET *p_sht, int height)
{
    int h, old;
    struct SHTCTL *p_ctl = GetShtctl();
	
    if( p_sht == NULL)
		return;
	
	if(p_sht->height == height)
		return;
	
//	if(p_sht->height | 0x80) {
//		old = p_sht->height - 0x100 ;
//	} else
	old = p_sht->height;
    if( height > p_ctl->top + 1)
    {
        height = p_ctl->top + 1;
    }
    
    if( height < -1)
    {
        height = -1;
    }
    p_sht->height = height;
   

    if( old > height)
    {
        if( height > 0)
        {
            for( h = old; h > height; h--)
            {
                p_ctl->arr_p_sheets[h] = p_ctl->arr_p_sheets[ h - 1];
                p_ctl->arr_p_sheets[h]->height = h;
            }
            
            p_ctl->arr_p_sheets[height] = p_sht;
            
//            sheet_refreshmap( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bvxsize,  p_sht->vy0 + p_sht->bvysize, height + 1);
//            Sheet_refreshsub( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bvxsize,  p_sht->vy0 + p_sht->bvysize, height + 1, old);
            
        }
        else        
        {
            if( p_ctl->top > old) 
            {
                
                for( h = old; h < p_ctl->top; h ++)
                {
                    p_ctl->arr_p_sheets[h] = p_ctl->arr_p_sheets[ h + 1];
                    p_ctl->arr_p_sheets[h]->height = h;
                }
               
                
            }
             p_ctl->top --;
//            sheet_refreshmap( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bvxsize,  p_sht->vy0 + p_sht->bvysize, 0);
//            Sheet_refreshsub( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bvxsize,  p_sht->vy0 + p_sht->bvysize, 0, old - 1);
            
        }
        
       
        
    }
    else if( old < height)
    {
        if( old >= 0)
        {
            for( h = old; h < height; h ++)
            {
                p_ctl->arr_p_sheets[h] = p_ctl->arr_p_sheets[ h + 1];
                p_ctl->arr_p_sheets[h]->height = h;
            }
            p_ctl->arr_p_sheets[height] = p_sht;
        }
        else 
        {
            for( h = p_ctl->top; h >= height; h--)
            {
                p_ctl->arr_p_sheets[h + 1] = p_ctl->arr_p_sheets[ h];
                p_ctl->arr_p_sheets[h + 1]->height = h + 1;
            }
            p_ctl->arr_p_sheets[height] = p_sht;
            p_ctl->top ++;
        }
        
//        sheet_refreshmap( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bvxsize,  p_sht->vy0 + p_sht->bvysize, height);
//        Sheet_refreshsub( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bvxsize,  p_sht->vy0 + p_sht->bvysize,height, height);
        
    }
    
    return;
    
}

int Sheet_is_hide(sheet *p_sht)
{
	if(p_sht->height < 0)
		return 1;
	else
		return 0;
	
}

void Sheet_refresh( struct SHEET *p_sht)
{
    struct SHTCTL *p_ctl = GetShtctl();
	struct SHEET *p;
	int i;
    if( p_sht->height < 0)
		return;
//	CLR_LCD();
	for( i = p_sht->height; i <= p_ctl->top; i++)
	{
		
		p = p_ctl->arr_p_sheets[ i];
		p->p_gp->vdraw( p->p_gp, &p->cnt, &p->area);
		Sheet_refreshsub( p);
		
	}
	Flush_LCD();
	
	
//    {
//		
//        
////        Sheet_refreshsub( p_ctl, p_sht->vx0 + bx0, p_sht->vy0 + by0, p_sht->vx0 + bx1, p_sht->vy0 + by1, p_sht->height,  p_sht->height);
//    }
    
}

void Sheet_slide( struct SHEET *p_sht)
{
	//todo: 增加坐标是否重叠判断，如果重叠要把比他更高的图层也要显示
	if((p_sht->height)< 0)
		return;
	p_sht->p_gp->vdraw( p_sht->p_gp, &p_sht->cnt, &p_sht->area);
	Sheet_refreshsub( p_sht);
	Flush_LCD();
    return;
    
}

void Sheet_force_slide( struct SHEET *p_sht)
{
	
	p_sht->p_gp->vdraw( p_sht->p_gp, &p_sht->cnt, &p_sht->area);
	Sheet_refreshsub( p_sht);
	Flush_LCD();
    return;
    
}
void Sheet_free( struct SHEET *p_sht)
{
    if( p_sht->height >= 0)
    {
        Sheet_updown(  p_sht, -1);
        
    }
    p_sht->flags = 0;
		p_sht->cnt.data = NULL;
		
    return;
    
}

//void Sheet_copy_area( struct SHEET *p_dstsht, struct SHEET *p_srcsht)
//{
//	
//	p_dstsht->area.x0 = p_srcsht->area.x0;
//	p_dstsht->area.x1 = p_srcsht->area.x1;
//	p_dstsht->area.x0 = p_srcsht->area.x0;
//	p_dstsht->area.x0 = p_srcsht->area.x0;

//	
//}
//shtDefCmd *Get_shtDefCmd(void)
//{
//	if( signal_cmd == NULL)
//	{
//		signal_cmd = shtDefCmd_new();
//		
//		
//	}
//	
//	return signal_cmd;
//	
//}

//CTOR( shtDefCmd)
//FUNCTION_SETTING( shtCmd.shtExcute, ShtDefualtExc);



//END_CTOR

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static void Sheet_refreshsub( struct SHEET *p_sht)
{
	short i, j;
	sheet **pp_shtRow0;
	sheet *p_shtCol;
	if( p_sht->subAtt.numSubCol == 0 || p_sht->subAtt.numSubRow == 0)
		return;
	
	
	
	for( i = 0; i < p_sht->subAtt.numSubRow; i++)
	{
		pp_shtRow0 =  p_sht->pp_sub + i * p_sht->subAtt.numSubCol;
		for( j = 0; j < p_sht->subAtt.numSubCol; j++)
		{
			p_shtCol = pp_shtRow0[j];
			if( p_shtCol == NULL)
				continue;
			if( GP_CKECK_EFF( p_sht->cnt.effects , EFF_FOCUS)) 
				p_shtCol->cnt.effects = GP_SET_EFF( p_shtCol->cnt.effects, EFF_FOCUS);
			else 
				p_shtCol->cnt.effects = GP_CLR_EFF( p_shtCol->cnt.effects, EFF_FOCUS);
			p_shtCol->p_gp->vdraw( p_shtCol->p_gp, &p_shtCol->cnt, &p_shtCol->area);
			
		}
		
	}
    
    
}




//void ShtDefualtExc( shtCmd *self, struct SHEET *p_sht, void *arg)
//{
//	
//}















