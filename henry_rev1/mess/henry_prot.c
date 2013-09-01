/***************************************************************************

        HENRY Prot I - brazilian document printer
        http://www.dataponto.com.br/protocoladores-prot1.html

        Driver by Felipe Sanches <juca@members.fsf.org>
        Technical info at https://www.garoa.net.br/wiki/HENRY

****************************************************************************/


#include "emu.h"
#include "includes/henry_prot.h"
#include "rendlay.h"

static ADDRESS_MAP_START(i80c31_prg, AS_PROGRAM, 8, henry_prot_state)
	AM_RANGE(0x0000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(i80c31_io, AS_IO, 8, henry_prot_state)

/*
CPU -> LATCH -> EPROM
A0 -> 0 -> A7
A1 -> 1 -> A6
A2 -> 2 -> A5
A3 -> 3 -> A4
A4 -> 4 -> A0 -> RS
A5 -> 5 -> A1 -> R/W
A6 -> 6 -> A2
A7 -> 7 -> A3
*/

//A4 = RS do display
//A5 = R/W do display
//(A11 == 0) && (A10 == 0) => CS do display
//(A14 == 1) && (A15 == 1) => enable do mux que seleciona periféricos
//11?? 00?? ??00 ???? write data
//11?? 00?? ??01 ???? write command
//11?? 00?? ??10 ???? read data
//11?? 00?? ??11 ???? read command
//mirror=0x33cf
  AM_RANGE(0xc000,0xc000) AM_MIRROR(0x33cf) AM_DEVWRITE("hd44780", hd44780_device, control_write)
  AM_RANGE(0xc010,0xc010) AM_MIRROR(0x33cf) AM_DEVWRITE("hd44780", hd44780_device, data_write)
  AM_RANGE(0xc020,0xc020) AM_MIRROR(0x33cf) AM_DEVREAD("hd44780", hd44780_device, control_read)
  AM_RANGE(0xc030,0xc030) AM_MIRROR(0x33cf) AM_DEVREAD("hd44780", hd44780_device, data_read)


	AM_RANGE(0x0000, 0x7fff) AM_MIRROR(0x8000) AM_RAM AM_SHARE("ram")

//	AM_RANGE(MCS51_PORT_P0, MCS51_PORT_P3) AM_READWRITE(henry_io_r, henry_io_w)
	AM_RANGE(MCS51_PORT_P0, MCS51_PORT_P3) AM_READ(henry_io_r)

//write: 0xc400 => U12 (?)
//write: 0xc800 => U11 (?)
//read:  0xc020 => display
//write: 0xc000 => display
//write: 0xc010 => display

//P1.4 => WhatchDog Input (after timeout resets CPU)

ADDRESS_MAP_END

static INPUT_PORTS_START( henry_prot )
	PORT_START("botoes")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Botao Preto Superior") PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Botao Preto Inferior") PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Botao Azul") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Detector de papel") PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Fim de curso XMIN") PORT_CODE(KEYCODE_E)
INPUT_PORTS_END

void henry_prot_state::machine_start()
{
}

void henry_prot_state::machine_reset()
{
}


READ8_MEMBER(henry_prot_state::henry_io_r)
{
	switch (offset)
	{
    case 0x00:
    {
//        printf("Read from P0 (return 0)\n");
      return 0;
    }
    case 0x01:
    {
      UINT8 value = (ioport("botoes")->read()) & 0x67;
//      printf("value:%02X\n", value);

      return value;
/*
      int value = 0x7;
      static int counter = 0;
      if (counter++ < 50){
        value = 0;
      } 

      if (counter++ > 100){
        counter = 0;
      } 

        printf("[%d] Read from P1\n", value);
      return value;
*/

    }
    case 0x02:
    {
  //      printf("Read from P3 (return 0)\n");
      return 0;
    }
    case 0x03:
    {
    //    printf("Read from P3 (return 0)\n");
      return 0;
    }
		default:    return 0;
	}
}

/*
WRITE8_MEMBER(henry_prot_state::henry_io_w)
{
  static UINT8 p0=0, p1=0, p2=0, p3=0;  
	switch (offset)
	{
		case 0x00:
		{
        if (data != p0){
          p0=data;
          printf("Write to P0: %02X\n", data);
        }
			break;
		}
		case 0x01:
		{
        if (data != p1){
          p1=data;
          if (data != 0xFF && data != 0xEF)
            printf("Write to P1: %02X\n", data);
        }
			break;
		}
		case 0x02:
		{
        if (data != p2){
          p2=data;
          printf("Write to P2: %02X\n", data);
        }
			break;
		}
		case 0x03:
		{
        if (data != p3){
          p3=data;
          printf("Write to P3: %02X\n", data);
        }
			break;
		}
	}
}
*/

void henry_prot_state::palette_init()
{
	palette_set_color(machine(), 0, MAKE_RGB(138, 146, 148));
	palette_set_color(machine(), 1, MAKE_RGB(92, 83, 88));
}

static const gfx_layout henry_prot_charlayout =
{
	5, 8,                   /* 5 x 8 characters */
	256,                    /* 256 characters */
	1,                      /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	{ 3, 4, 5, 6, 7},
	{ 0, 8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8},
	8*8                     /* 8 bytes */
};

static GFXDECODE_START( henry_prot )
	GFXDECODE_ENTRY( "hd44780:cgrom", 0x0000, henry_prot_charlayout, 0, 1 )
GFXDECODE_END

INTERRUPT_GEN_MEMBER(henry_prot_state::interrupcao_t1_gerada_pelo_CI_desconhecido_U2)
{
//	m_maincpu->set_input_line(MCS51_T1_LINE, 0);
}

static MACHINE_CONFIG_START( henry_prot, henry_prot_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I80C31, 10000000) // 10MHz
	MCFG_CPU_PROGRAM_MAP(i80c31_prg)
	MCFG_CPU_IO_MAP(i80c31_io)
  MCFG_CPU_PERIODIC_INT_DRIVER(henry_prot_state, interrupcao_t1_gerada_pelo_CI_desconhecido_U2,  1250000) /* Hz (frequencia tmb desconhecida)*/

	/* video hardware */
	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_UPDATE_DEVICE("hd44780", hd44780_device, screen_update)
	MCFG_SCREEN_SIZE(6*16, 9*2)
	MCFG_SCREEN_VISIBLE_AREA(0, 6*16-1, 0, 9*2-1)
	MCFG_DEFAULT_LAYOUT(layout_lcd)
	MCFG_PALETTE_LENGTH(2)
	MCFG_GFXDECODE(henry_prot)

	MCFG_HD44780_ADD("hd44780")
	MCFG_HD44780_LCD_SIZE(2, 16)

//There seems to be an eeprom or a clock placed at U2 (DIP8)
//pin1 -> 8031 pin 14 (T0: Timer 0 external input) 
//pin2 -> cristal at X2 (labeled 32.768)
//pin3 -> ?
//pin4 -> GND
//pin5 -> ?
//pin6 -> 8031 pin 5 (Port 1 bit 4)
//pin7 -> 8031 pin 4 (Port 1 bit 3)
//pin8 -> VCC

MACHINE_CONFIG_END



/* ROM definition */
ROM_START( henry_prot )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "henry_protv19_desobfuscado.bin",  0x00000, 0x10000, CRC(df39004e) SHA1(c0a83496fc98767de263133809c8c63fd43549b8) )
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT    INIT COMPANY   FULLNAME       FLAGS */
COMP( 2004, henry_prot,  0,     0, henry_prot,     henry_prot,    driver_device,    0,   "henry_prot",   "HENRY Prot V19 (REV.1)", GAME_IMPERFECT_GRAPHICS | GAME_NO_SOUND)
