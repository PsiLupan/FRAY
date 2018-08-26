#include "hsd_lobj.h"

static void LObjInfoInit(void);

HSD_LObjInfo hsdLObj = { LObjInfoInit };

static HSD_LObjInfo *default_class = NULL;

static HSD_SList *current_lights = NULL;
static HSD_LObj  *active_lights[MAX_GXLIGHT];
static int        nb_active_lights = 0;

static GXLightID lightmask_diffuse  = GX_LIGHT_NULL;
static GXLightID lightmask_specular = GX_LIGHT_NULL;
static GXLightID lightmask_attnfunc = GX_LIGHT_NULL;
static GXLightID lightmask_alpha    = GX_LIGHT_NULL;

