<Qucs Schematic 0.0.15>
<Properties>
  <View=0,-120,870,882,1,0,0>
  <Grid=10,10,1>
  <DataSet=cdiv.dat>
  <DataDisplay=cdiv.dpl>
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
  <GND * 1 220 400 0 0 0 0>
  <GND * 1 100 400 0 0 0 0>
  <Vrect V1 1 100 310 18 -26 0 1 "3.3 V" 1 "31 ns" 1 "31 ns" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0>
  <.DC DC1 1 120 40 0 36 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.TR TR1 1 310 40 0 57 0 0 "lin" 1 "0" 1 "1 us" 1 "10000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <C C1 1 220 350 17 -26 0 1 "220 pF" 1 "" 0 "neutral" 0>
  <R R1 1 150 200 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C2 1 220 250 17 -26 0 1 " 33 pF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <100 340 100 400 "" 0 0 0 "">
  <220 280 220 320 "Vout" 250 270 17 "">
  <220 380 220 400 "" 0 0 0 "">
  <100 200 100 280 "" 0 0 0 "">
  <100 200 120 200 "" 0 0 0 "">
  <180 200 220 200 "" 0 0 0 "">
  <220 200 220 220 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 360 416 414 196 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"Vout.Vt" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect 360 683 421 203 3 #c0c0c0 1 00 1 0 0.2 1 1 -0.1 0.5 1.1 1 -0.1 0.5 1.1 315 0 225 "" "" "">
	<"V1.It" #0000ff 0 3 0 0 0>
  </Rect>
  <Tab 530 210 300 200 3 #c0c0c0 1 00 1 923 1 1 1 0 1 1 1 0 1 10000 315 0 225 "" "" "">
	<"Vout.Vt" #0000ff 0 3 1 0 0>
  </Tab>
</Diagrams>
<Paintings>
</Paintings>
