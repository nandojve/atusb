<Qucs Schematic 0.0.15>
<Properties>
  <View=0,0,1010,882,1,0,0>
  <Grid=10,10,1>
  <DataSet=clk.dat>
  <DataDisplay=clk.dpl>
  <OpenDisplay=1>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 560 480 0 0 0 0>
  <.DC DC1 1 90 40 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <Vrect V2 1 560 390 18 -26 0 1 "3.3 V" 1 "33 ns" 1 "33 ns" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0>
  <.TR TR1 1 100 120 0 57 0 0 "lin" 1 "0" 1 "0.1 us" 1 "9991" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R3 1 490 260 -26 15 0 0 "100 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 420 480 0 0 0 0>
  <C C2 1 420 370 17 -26 0 1 "50 pF" 1 "" 0 "neutral" 0>
  <GND * 1 280 480 0 0 0 0>
  <R R1 1 280 330 15 -26 0 1 "56 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R2 1 280 430 15 -26 0 1 "10 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 350 260 -26 17 0 0 "22 pF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <520 260 560 260 "" 0 0 0 "">
  <560 260 560 360 "" 0 0 0 "">
  <560 420 560 480 "" 0 0 0 "">
  <420 400 420 480 "" 0 0 0 "">
  <420 260 460 260 "" 0 0 0 "">
  <420 260 420 340 "" 0 0 0 "">
  <380 260 420 260 "Vmeas" 350 140 39 "">
  <280 360 280 400 "Vout" 170 330 17 "">
  <280 260 280 300 "" 0 0 0 "">
  <280 260 320 260 "" 0 0 0 "">
  <280 460 280 480 "" 0 0 0 "">
  <560 260 560 260 "Vin" 590 200 0 "">
</Wires>
<Diagrams>
  <Rect 120 752 719 212 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"Vin.Vt" #0000ff 0 3 0 0 0>
	<"Vmeas.Vt" #ff0000 0 3 0 0 0>
	<"Vout.Vt" #ff00ff 0 3 0 0 0>
  </Rect>
  <Rect 670 480 278 250 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"Vout.Vt" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
