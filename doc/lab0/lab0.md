# Lab 0: De omgeving

## Opzet

In dit labo maak je kennis met de ontwikkelomgeving voor de microcontroller en de bijhorden software flow.

## STM32L4

Het labo is opgebouwd met de STM32L452 microcontroller van STMicroelectronics. Deze microcontroller komt uit de STM32L4 lijn, deze lijn van controllers zijn speciaal gemaakt voor ultra low power toepassingen. Daarvoor heeft de microcontroller allerlei geavanveerde pereferie ingebouwd welke oftwel het verbruik optimaliseren of deze zo minimaal mogelijk energie verbruiken. 

Een aantal belangrijke kenmerken van de microcontroller:
* ARM Cortex M4F processor met een maximale kloksnelheid van 80 MHz
* 512 kiB flash
* 160 kiB SRAM
* ADC/DAC
* 5 UARTs blokken
* 3 SPIs blokken en ook een QSPI blok
* 4 I2Cs blokken
* 7 Timers
* USB en CAN
* SDIO (SD kaart interface)
* Watchdogs
* Random Number Generator
* 2 DMA controllers

Zoals men kan zien, zitten deze microcontrollers boordevol features en mogelijkheden. Daardoor zijn deze type van microcontrollers zeer veelzijdig in te zetten, van ultra low power sensoren om de groei van planten te optimaliseren tot intiligenten voertuig monitoring om ongevallen te vermeiden. 

In onderstaand diagramma geeft blokschematisch weer hoe de microcontroller die gebruikt wordt in het labo van binnenuit ziet. 

![Blok diagramma](img/ds_stm32l452xx.PNG)

In het diagramma kunnen we de volgende onderdelen herkennen:

* De processor
* Het geheugen
* De perferie, UARTs/SPIs/...
* De bussen die alles met elkaar verbinden

### Bussen

Omwillen van de complexiteit en mogelijkheden tot optimalisatie van zijn er verschillende bussen gebruikt op de microcontroller.

AHB(Arm High performance Bus) bus-matrix is verbind de processor met het Flash en RAM geheugen en de AHB1 en AHB2 bussen. Ook heeft deze connecties met de DMA controllers en de QSPI interface. In deze bus-matrix zijn de processor en de DMA controllers de master en zijn het geheugen samen met de AHB1 en AHB2 bussen de slaves. Elke de masters kunnen informatie opvragen bij de slaves. 

De AHB1 en AHB2 bussen verbinden de peferie blokken met met de busmatrix. Dit kan direct zijn zoals de GPIO peferie of indirect via een AHB-to-APB bus omvormer. De meeste hardware blokken zijn dan ook verbonden met de een APB(Arm perhifial bus) bus.

Om straks een hardware blok te gebruiken moet daar meestal eerst een intern klok voor geactiveerd worden. Om te weten welke klok dit is, is het diagramma een handige tool omdat deze klokken gegroepeert zijn per bus waarmee deze hardware blokken zijn verbonden. 

### ARM Cortex M4F

De ARM Cortex M4 processor gebruikt de ARMv7E-M architetuur en ondersteunt de volledige Thumb-1 en Thumb-2 instructies sets. 

Verder bevat de Cortex M4 de volgende features:
* 32-bit single-cycle hardware integer vermenigvulder met 32-bit of 64-bit resultaat, met of zonder teken, add off subtract na de vermenigvuldiging.
* 32-bit single-cycle MAC(multiply accumulate).
* 32-bit hardware integer deling (2–12 cycles)
* Saturatie wiskunde support
* DSP extenties: 
    * Single cycle 16/32-bit MAC, 
    * single cycle dual 16-bit MAC, 
    * 8/16-bit SIMD instructies
* 1 to 240 interrupts, plus NMI
* 12 cycle interrupt latency
* floating-point unit (FPU): alleen single-precision, IEEE-754 compliant
* Memory protection unit (MPU)

## Software tools

Voor het labo gebruiken we de STM32CubeIDE versie 1.8.0. Deze tool is te vinden op: https://www.st.com/en/development-tools/stm32cubeide.html

In het labo gebruiken we rechtstreeks de hardware registers in plaats van de HAL bibliotheek. Het template project dat gebruikt moet worden voor het labo bevat de juiste initilasatie en configuratie. 

### Make & Makefiles

Make is een computerprogramma dat als hulpmiddel kan worden gebruikt om grote applicaties automatisch te bouwen voor softwareontwikkeling. De computerbestanden die instructies bevatten voor make worden makefiles genoemd. Make houdt bij welke bestanden zijn gewijzigd sinds de laatste maal dat een project was gebouwd en start de compiler enkel voor deze broncodebestanden en de bestanden die afhankelijk zijn van deze bestanden.

Het template project gebruikt Makefiles voor configuratie van de compelatie. In deze MakeFile staat ook welke files gecompileerd moeten worden tot één werkende applicatie. Als men het pad van het C bestand toevoegt aan C_SOURCES wordt deze mee genomen in de compelatie. Een map toevoegen aan C_INCLUDES maakt de .h files beschikbaar voor include statements.

    ######################################
    # source
    ######################################
    # C sources
    C_SOURCES =	startup/system.c \
                src/main.c \
                src/interrupts.c \
                src/syscalls.c \
                src/sysmem.c 

    # C includes
    C_INCLUDES =	-Isrc \
                    -Ilib \
                    -Ilib/cmsis 

De Makefile bevat ook de configuratie over voor welke archectuur de C code moet worden gecompiled. De **-mcpu=cortex-m4** flag zegt tegen de C compiler dat het over een ARM Cortex M4 processor gaat, de **-mfpu=fpv4-sp-d16** maakt aan de compiler duidelijk dat de microcontroller een floating point unit heeft. 

    #######################################
    # CFLAGS
    #######################################
    CPU = -mcpu=cortex-m4
    FPU = -mfpu=fpv4-sp-d16
    FLOAT-ABI = -mfloat-abi=hard
    MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)


## Hardware tools

Voor dit labo wordt er een speciaal ontworpen bordje ter beschikkinggesteld. Dit bevat alle nodige rand componenten om de microcontroller te laten werken en de verschillende opgaves mee te maken en testen.

### Microcontroller

![Bordje Microcontroller](img/testboard_mcu.jpg)

In de rode cirkel op bovenstaande foto bevindt zich de mircocontroller. Dit is een STM32L452 in een QFN behuizing. 

De blauwe cirkel bevat de RESET knop waarmee de processor gereset kan worden. Ook is er een DFU knopje, hiermee kan de microcontroller in USB bootloader modus worden geconfigureerd. Om in deze modus te geraken moet men de DFU knop inhouden en dan kort de RESET knop indrukken. 

De debug verbinding bevindt zich in de groene cirkel. Hiermee wordt via het flat kabeltje het testbordje verbonden met de debugger. 

Via de USB connector of de JST PH stekker in de gele cirkel kan het testbordje worden gevoed. De JST PH connector ondersteunt een 1 cell Li-Ion batterij. Op het bordje zit een batterij lader ingebouwd welke via de USB poort de Li-Ion batterij geladen kan worden.

### UI
![Blok diagramma](img/testboard_ui.jpg)

In de rode cirkel bevinden zich twee LEDs welke kunnen aangestuurd worden door de microcontroller.

| Functie | GPIO | Pin | 
| ------- | ---- | --- |
| LED1 | GPIOB | Pin 9 |
| LED2 | GPIOC | Pin 13 |

In de groene cirkel staan twee knopjes.

| Functie | GPIO | Pin | 
| ------- | ---- | --- |
| Button A | GPIOB | Pin 13 |
| Button B | GPIOB | Pin 14 |

In de blauwe cirkel staan een 10 kOhm NTC weerstand en een Potention meter. Deze twee zijn verbonden met een ADC input van de microcontroller.

| Functie | GPIO | Pin |  ADC Kanaal |
| ------- | ---- | --- | ----------- |
| Potentionmeter | GPIOA | Pin 1 | CH5 |
| NTC weerstand  | GPIOA | Pin 0 | CH6 |


Het 7 segment scherm in de gele cirkel wordt gemultiplext. Via de volgende pinnen kan het 7 segment scherm worden aangestuurd:

| Functie | GPIO | Pin |
| ------- | ---- | --- |
| Disp 0 | GPIOA | Pin 8 |
| Disp 1 | GPIOA | Pin 15 | 
| Seg A | GPIOB | Pin 0 | 
| Seg B | GPIOA | Pin 7 | 
| Seg C | GPIOA | Pin 5 | 
| Seg D | GPIOB | Pin 12 | 
| Seg E | GPIOB | Pin 15 | 
| Seg F | GPIOB | Pin 1 | 
| Seg G | GPIOB | Pin 2 | 
| Seg DP | GPIOA | Pin 6 | 


### Buzzer & IO
![Blok diagramma](img/testboard_buz_io.jpg)

### I2C
![Blok diagramma](img/testboard_i2c.jpg)

### LR1110
![Blok diagramma](img/testboard_lr1110.jpg)

### Debugger

## Opdracht Compile and Debug

### Project importeren

### Compilen

### Debuggen

### Registers bekijken

### Disassembly bekijken

