# Eleph Hardware
This folder contains hardware design of the ELEPH

## Design files
In the "**KiCad**" folder you can find KiCad project of the Eleph.

## Schematic
In the "**Schematic**" folder you can find PDF file with the schematic diagram.

## Production files
In the "**Gerbers**" folder you can find production files of Eleph.

## Hardware Notes
Couple of words about power supply. Supply chain is **_Single 18650 -> LDO 3.3V_**. I found no reason to put DC/DC because in common they have 80%-90% of efficiency, while LDO has ~78% in worst case (3.3V / 4.2V = 0.78). Do the increased complexity and cost worth several percent of efficiency? I do not think so.
