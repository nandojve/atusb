<!-- Linux ben-wpan production testing Web page style -->

<!-- Written 2011 by Werner Almesberger -->


<MACRO name="__PI" bgcolor size="+2" rowspan="1">
  <TD align="center" valign="top" rowspan="<ROWSPAN>">
  <TABLE border=0 cellspacing=3 cellpadding=2 bgcolor="<BGCOLOR>">
    <TR><TD nowrap="nowrap"><FONT size="<SIZE>"><B><BODY></B>
  </TABLE>
</MACRO>

<MACRO name="__PI_SEP">
  <TD align="center" bgcolor="#000000">
  <TABLE border=0 cellspacing=0 cellpadding=1>
    <TR><TD>
  </TABLE>
</MACRO>

<MACRO name="PAGE_BAR" title>
  <TABLE width="99%" border=0 cellspacing=0 cellpadding=0>
    <TR>
    <__PI bgcolor="#d080ff" size="+3" rowspan="2"><A
      href="index.html"><B><TITLE></B></A></__PI>
    <TD align="left" width="99%">
    <TABLE border=0 cellspacing=0 cellpadding=0>
      <TR>
      <BODY>
      <__PI_SEP>
    <TD align="left" width="99%" bgcolor="#d080ff">
    </TABLE>
</MACRO>

<MACRO name="PAGE_ITEM" href>
  <__PI_SEP>
  <__PI bgcolor="#c0d0ff"><A href="<HREF>"><BODY></A></__PI>
</MACRO>

<MACRO name="PAGE_CURR" href>
  <__PI_SEP>
  <__PI bgcolor="#ffff00"><BODY></__PI>
</MACRO>

<MACRO name="SECTION_BAR" title>
    <TR><!-- <TD align="left" bgcolor="#ffffff"> -->
    <TD align="left">
    <TABLE border=0 cellspacing=0 cellpadding=0>
      <TR>
      <BODY>
      <__PI_SEP>
    <TD align="left" width="99%" bgcolor="#d080ff">
    </TABLE>
  </TABLE>
</MACRO>

<MACRO name="SECTION_ITEM" href>
  <__PI_SEP>
  <TD align="center">
  <TABLE border=0 cellspacing=3 cellpadding=2 bgcolor="#ffff00">
    <TR><TD nowrap="nowrap"><A href="<HREF>"<FONT size="+1"><B><BODY></B></A>
  </TABLE>
</MACRO>

<MACRO name="SECTION" ref title>
  <P>
  <TABLE width="99%" border=0 cellspacing=0 cellpadding=2 bgcolor="#f0f0a0">
    <TR><TD>&nbsp;<A name="<REF>"><FONT
    size="+2"><B><TITLE></B></FONT></A></TD>
  </TABLE></TD>
  <P>
</MACRO>

<MACRO name="SUBSECTION" title>
<P>
<B><FONT size="+1"><TITLE></B></FONT>
<P>
</MACRO>
