/***************************************************************************

        HENRY Prot

****************************************************************************/

#pragma once

#ifndef _HENRYPROT_H_
#define _HENRYPROT_H_

#include "cpu/mcs51/mcs51.h"
#include "video/hd44780.h"

// ======================> henry_prot_state

class henry_prot_state : public driver_device
{
public:
	henry_prot_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_lcdc(*this, "hd44780"),
			m_ram(*this, "ram"){ }

	required_device<cpu_device> m_maincpu;
	required_device<hd44780_device> m_lcdc;

	required_shared_ptr<UINT8> m_ram;
	UINT8 *m_paged_ram;

	virtual void machine_start();
	virtual void machine_reset();

	void io_rw(address_space &space, UINT16 offset);
	virtual void palette_init();

	DECLARE_WRITE8_MEMBER(henry_io_w);
	DECLARE_READ8_MEMBER(henry_io_r);
	static HD44780_PIXEL_UPDATE(pixel_update);

	INTERRUPT_GEN_MEMBER(interrupcao_t1_gerada_pelo_CI_desconhecido_U2);
};

#endif  // _HENRYPROT_H_
