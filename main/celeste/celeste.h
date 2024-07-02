#ifndef CELESTE_H_
#define CELESTE_H_

#ifdef __cplusplus
#define _Bool bool
extern "C"
{
#endif

	typedef enum
	{
		CELESTE_P8_SPR,
		CELESTE_P8_BTN,
		CELESTE_P8_PAL,
		CELESTE_P8_PAL_RESET,
		CELESTE_P8_CIRCFILL,
		CELESTE_P8_PRINT,
		CELESTE_P8_RECTFILL,
		CELESTE_P8_LINE,
		CELESTE_P8_MGET,
		CELESTE_P8_CAMERA,
		CELESTE_P8_FGET,
		CELESTE_P8_MAP
	} CELESTE_P8_CALLBACK_TYPE;

	typedef _Bool Celeste_P8_bool_t;

	void Celeste_P8_set_rndseed(unsigned seed);
	void Celeste_P8_init(void);
	void Celeste_P8_update(void);
	void Celeste_P8_draw(void);
	void Celeste_P8_test();

	void Celeste_P8__DEBUG(void); // debug functionality

	// state functionality
	size_t Celeste_P8_get_state_size(void);
	void Celeste_P8_save_state(void *st);
	void Celeste_P8_load_state(const void *st);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
