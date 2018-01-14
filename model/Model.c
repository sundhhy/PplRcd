#include "Model.h"
#include "sdhDef.h"

#include <string.h>
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T model_Failed = { "MDL Failed" };
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

	

static const char *su_Nm3_h = "Nm3/h";
static const char *su_m3_h = "m3/h";
static const char *su_m3_min = "m3/min";
static const char *su_m3_s = "m3/s";
static const char *su_L_h = "L/h";
static const char *su_L_min = "L/min";
static const char *su_L_s = "L/s";
static const char *su_kg_h = "kg/h";
static const char *su_kg_min = "kg/min";
static const char *su_kg_s = "kg/s";
static const char *su_oC = "℃";
static const char *su_Pa = "Pa";
static const char *su_MPa = "MPa";
static const char *su_kgf_cm2 = "kgf/cm2";
static const char *su_Bar = "Bar";
static const char *su_mmH2O = "mmH2O";
static const char *su_mmHg = "mmHg";
static const char *su_percent = "%";			//%
static const char *su_ppm = "ppm";
static const char *su_pH = "pH";
static const char *su_r_min = "r/min";
static const char *su_Hz = "Hz";
static const char *su_kHz = "kHz";
static const char *su_mA = "mA";
static const char *su_A = "A";
static const char *su_kA = "kA";
static const char *su_mV = "mV";
static const char *su_V = "V";
static const char *su_kV = "kV";
static const char *su_W = "W";
static const char *su_kW = "kW";
static const char *su_MW = "MW";
static const char *su_Var = "Var";
static const char *su_kVar = "kVar";
static const char *su_MVar = "MVar";
static const char *su_J = "J";
static const char *su_kJ = "kJ";
static const char *su_uS_cm = "uS/cm";
static const char *su_J_kgoC = "J/kg℃";
static const char *su_kWh = "kWh";
static const char *su_ug_L = "ug/L";
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */
static void DoUpdate(void **x, void *cl)
{
	Observer *s = ( Observer *)*x;
	s->update(s, cl);
	
}

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//




void Pe_float(int data, int prec, char *str)
{
	int		d1, d2;
	int		sign = 1;
	
	if(data < 0)
		sign = -1;
	
	if(prec == 1) {
		d1 = data/10;
		d2 = data%10;
		
		//显示数字的符号时，把符号作为整体的符号来显示
		//去除每个部分的符号，
		d1 = d1 * sign;
		d2 = d2 * sign;
		if(sign > 0)
			sprintf(str, "%d.%d", d1, d2);
		else 
			sprintf(str, "-%d.%d", d1, d2);
		
	}
	else if(prec == 2)  {
		d1 = data/100;
		d2 = data%100;
		
		//显示数字的符号时，把符号作为整体的符号来显示
		//去除每个部分的符号，
		d1 = d1 * sign;
		d2 = d2 * sign;
		if(sign > 0)
			sprintf(str, "%d.%02d", d1, d2);
		else 
			sprintf(str, "-%d.%02d", d1, d2);
		
	}
	
	
}

void Pe_frefix_float(int data, int prec, char *prefix, char *str)
{
	
	if(prec == 1)
		sprintf(str, "%s%d.%d", prefix, data/10, data%10);
	else if(prec == 2) 
		sprintf(str, "%s%d.%02d", prefix, data/100, data%100);
}

void Mdl_unit_to_string( char unit, char *buf, int len)
{
	switch( unit)
	{
		case eu_Nm3_h:
			strncpy( buf,su_Nm3_h, len);
			break;
		case eu_m3_h:
			strncpy( buf,su_m3_h, len);
			break;
		case eu_m3_min:
			strncpy( buf,su_m3_min, len);
			break;
		case eu_m3_s:
			strncpy( buf,su_m3_s, len);
			break;
		case eu_L_h:
			strncpy( buf,su_L_h, len);
			break;
		case eu_L_min:
			strncpy( buf,su_L_min, len);
			break;	
		case eu_L_s:
			strncpy( buf,su_L_s, len);
			break;
		case eu_kg_h:
			strncpy( buf,su_kg_h, len);
			break;
		case eu_kg_min:
			strncpy( buf,su_kg_min, len);
			break;
		case eu_kg_s:
			strncpy( buf,su_kg_s, len);
			break;
		case eu_oC:
			strncpy( buf,su_oC, len);
			break;
		case eu_Pa:
			strncpy( buf,su_Pa, len);
			break;	
		case eu_MPa:
			strncpy( buf,su_MPa, len);
			break;
		case eu_kgf_cm2:
			strncpy( buf,su_kgf_cm2, len);
			break;
		case eu_Bar:
			strncpy( buf,su_Bar, len);
			break;
		case eu_mmH2O:
			strncpy( buf,su_mmH2O, len);
			break;
		case eu_mmHg:
			strncpy( buf,su_mmHg, len);
			break;
		case eu_percent:
			strncpy( buf,su_percent, len);
			break;	
		case eu_ppm:
			strncpy( buf,su_ppm, len);
			break;
		case eu_pH:
			strncpy( buf,su_pH, len);
			break;
		case eu_r_min:
			strncpy( buf,su_r_min, len);
			break;
		case eu_Hz:
			strncpy( buf, su_Hz, len);
			break;
		case eu_kHz:
			strncpy( buf,su_kHz, len);
			break;
		case eu_mA:
			strncpy( buf,su_mA, len);
		case eu_A:
			strncpy( buf,su_A, len);
			break;
		case eu_kA:
			strncpy( buf,su_kA, len);
			break;
		case eu_mV:
			strncpy( buf,su_mV, len);
			break;
		case eu_V:
			strncpy( buf,su_V, len);
			break;
		case eu_kV:
			strncpy( buf,su_kV, len);
			break;
		case eu_W:
			strncpy( buf,su_W, len);
			break;	
		case eu_kW:
			strncpy( buf,su_kW, len);
			break;
		case eu_MW:
			strncpy( buf,su_MW, len);
			break;
		case eu_Var:
			strncpy( buf,su_Var, len);
			break;
		case eu_kVar:
			strncpy( buf,su_kVar, len);
			break;
		case eu_MVar:
			strncpy( buf,su_MVar, len);
			break;	
		case eu_J:
			strncpy( buf,su_J, len);
			break;
		case eu_kJ:
			strncpy( buf,su_kJ, len);
			break;
		case eu_uS_cm:
			strncpy( buf,su_uS_cm, len);
			break;
		case eu_J_kgoC:
			strncpy( buf,su_J_kgoC, len);
			break;
		case eu_kWh:
			strncpy( buf,su_kWh, len);
			break;
		case eu_ug_L:
			strncpy( buf,su_ug_L, len);			
			
			
			
			break;		
		default:
			strncpy( buf, "m3/h", len);
			break;
		
	}
	
}

int Mdl_string_to_unit( char *s_unit, int len)
{
	char *p;
	int		u = -1;
	
	p = strstr(s_unit, su_Nm3_h);
	if(p) {
		u = eu_Nm3_h;
		goto exit;
	}
	p = strstr(s_unit, su_m3_h);
	if(p) {
		u = eu_m3_h;
		goto exit;
	}
	p = strstr(s_unit, su_m3_min);
	if(p) {
		u = eu_m3_min;
		goto exit;
	}
	p = strstr(s_unit, su_m3_s);
	if(p) {
		u = eu_m3_s;
		goto exit;
	}		
	p = strstr(s_unit, su_L_h);
	if(p) {
		u = eu_L_h;
		goto exit;
	}
	p = strstr(s_unit, su_L_min);
	if(p) {
		u = eu_L_min;
		goto exit;
	}
	p = strstr(s_unit, su_L_s);
	if(p) {
		u = eu_L_s;
		goto exit;
	}
	p = strstr(s_unit, su_kg_h);
	if(p) {
		u = eu_kg_h;
		goto exit;
	}		
	p = strstr(s_unit, su_kg_min);
	if(p) {
		u = eu_kg_min;
		goto exit;
	}
	p = strstr(s_unit, su_kg_s);
	if(p) {
		u = eu_kg_s;
		goto exit;
	}
	p = strstr(s_unit, su_oC);
	if(p) {
		u = eu_oC;
		goto exit;
	}
	p = strstr(s_unit, su_Pa);
	if(p) {
		u = eu_Pa;
		goto exit;
	}		
	p = strstr(s_unit, su_MPa);
	if(p) {
		u = eu_MPa;
		goto exit;
	}
	p = strstr(s_unit, su_kgf_cm2);
	if(p) {
		u = eu_kgf_cm2;
		goto exit;
	}
	p = strstr(s_unit, su_Bar);
	if(p) {
		u = eu_Bar;
		goto exit;
	}
	p = strstr(s_unit, su_mmH2O);
	if(p) {
		u = eu_mmH2O;
		goto exit;
	}	
		p = strstr(s_unit, su_mmHg);
	if(p) {
		u = eu_mmHg;
		goto exit;
	}
	p = strstr(s_unit, su_percent);
	if(p) {
		u = eu_percent;
		goto exit;
	}
	p = strstr(s_unit, su_ppm);
	if(p) {
		u = eu_ppm;
		goto exit;
	}
	p = strstr(s_unit, su_pH);
	if(p) {
		u = eu_pH;
		goto exit;
	}	
	
	
	
	p = strstr(s_unit, su_r_min);
	if(p) {
		u = eu_r_min;
		goto exit;
	}
	p = strstr(s_unit, su_Hz);
	if(p) {
		u = eu_Hz;
		goto exit;
	}
	p = strstr(s_unit, su_mA);
	if(p) {
		u = eu_mA;
		goto exit;
	}
	p = strstr(s_unit, su_A);
	if(p) {
		u = eu_A;
		goto exit;
	}		
	p = strstr(s_unit, su_kA);
	if(p) {
		u = eu_kA;
		goto exit;
	}
	p = strstr(s_unit, su_mV);
	if(p) {
		u = eu_mV;
		goto exit;
	}
	p = strstr(s_unit, su_V);
	if(p) {
		u = eu_V;
		goto exit;
	}
	p = strstr(s_unit, su_kV);
	if(p) {
		u = eu_kV;
		goto exit;
	}		
	p = strstr(s_unit, su_W);
	if(p) {
		u = eu_W;
		goto exit;
	}
	p = strstr(s_unit, su_kW);
	if(p) {
		u = eu_kW;
		goto exit;
	}
	p = strstr(s_unit, su_MW);
	if(p) {
		u = eu_MW;
		goto exit;
	}
	p = strstr(s_unit, su_Var);
	if(p) {
		u = eu_Var;
		goto exit;
	}		
	p = strstr(s_unit, su_kVar);
	if(p) {
		u = eu_kVar;
		goto exit;
	}
	p = strstr(s_unit, su_MVar);
	if(p) {
		u = eu_MVar;
		goto exit;
	}
	p = strstr(s_unit, su_J);
	if(p) {
		u = eu_J;
		goto exit;
	}
	p = strstr(s_unit, su_kJ);
	if(p) {
		u = eu_kJ;
		goto exit;
	}	
		p = strstr(s_unit, su_uS_cm);
	if(p) {
		u = eu_uS_cm;
		goto exit;
	}
	p = strstr(s_unit, su_J_kgoC);
	if(p) {
		u = eu_J_kgoC;
		goto exit;
	}
	p = strstr(s_unit, su_kWh);
	if(p) {
		u = eu_kWh;
		goto exit;
	}
	p = strstr(s_unit, su_ug_L);
	if(p) {
		u = eu_ug_L;
		goto exit;
	}	
	
	exit:
	return u;
	
}










void Mdl_attach(  Model *self, Observer *s)
{
	
	self->tObs = List_push( self->tObs, s);
	
	
}

void Mdl_detach(  Model *self, Observer *s)
{
	
	
	
}

int Mdl_getData(  Model *self, IN int aux, void *arg) 
{
	memcpy( self->coreData, arg, self->crDt_len);
	return RET_OK;
}

int Mdl_setData(  Model *self, IN int aux,  void *arg) 
{
	memcpy( arg, self->coreData, self->crDt_len);
	return RET_OK;
}


int	Mdl_addTmMdl( Model *self, Model *m)
{
	self->teamMdl = m;
	return RET_OK;

}	

int Mdl_delTmMdl( Model *self, Model *m)
{
	self->teamMdl = NULL;
	return RET_OK;
}
//int Mdl_installDataSource( Model *self, void *dsr)
//{
//	self->dataSource = dsr;
//	return RET_OK;
//}

void Mdl_notify (Model *self)
{
	
	
	
	List_map(self->tObs, DoUpdate, self);
}



//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

ABS_CTOR( Model)
FUNCTION_SETTING( attach, Mdl_attach);
FUNCTION_SETTING( detach, Mdl_detach);
FUNCTION_SETTING( getMdlData, Mdl_getData);
FUNCTION_SETTING( setMdlData, Mdl_setData);
FUNCTION_SETTING( addTmMdl, Mdl_addTmMdl);
FUNCTION_SETTING( delTmMdl, Mdl_delTmMdl);
//FUNCTION_SETTING( installDataSource, Mdl_installDataSource);
FUNCTION_SETTING( notify, Mdl_notify);

END_ABS_CTOR


