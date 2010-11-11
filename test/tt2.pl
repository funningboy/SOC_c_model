  use GraphViz::XML;

  $xml = '<Report>
  <Rpt_Note>
    <ReportType>timing</ReportType>
  </Rpt_Note>
  <PathList>
     <Path>
        <Path_Node>
            <StartPoint>xxx.xxx.xxx</StartPoint>
            <EndPoint>ccc.ccc.ccc</EndPoint>
            <DelayType>Max</DelayType>
        </Path_Node>
        <Point>
            <PointType>NetOrPort</PointType>
            <Name>xxx.xxx.xxx</Name>
            <Trigger>Rising</Trigger>
            <IncDelay>0.000</IncDelay>
            <TotalDelay>9.623</TotalDelay>
         </Point>
         <Point>
            <PointType>NetOrPort</PointType>
            <Name>ccc.ccc.ccc</Name>
            <Trigger>Falling</Trigger>
            <IncDelay>0.100</IncDelay>
            <TotalDelay>9.723</TotalDelay>
         </Point>
     </Path>
  </PathList>
</Report>';

  my $graph = GraphViz::XML->new($xml);
  print $g->as_png;
