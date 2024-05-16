<?xml version="1.0" encoding="UTF-8"?>
<simconf version="2023090101">
  <simulation>
    <title>Greenhouse</title>
    <speedlimit>2.0</speedlimit>
    <randomseed>123456</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      org.contikios.cooja.radiomediums.UDGM
      <transmitting_range>50.0</transmitting_range>
      <interference_range>100.0</interference_range>
      <success_ratio_tx>1.0</success_ratio_tx>
      <success_ratio_rx>1.0</success_ratio_rx>
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      org.contikios.cooja.mspmote.Z1MoteType
      <description>Gateway</description>
      <source>[CONFIG_DIR]/gateway/gateway.c</source>
      <commands>$(MAKE) -j$(CPUS) gateway.z1 TARGET=z1</commands>
      <firmware>[CONFIG_DIR]/gateway/build/z1/gateway.z1</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="73.94707398160425" y="6.742617094150319" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>1</id>
        </interface_config>
      </mote>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.Z1MoteType
      <description>SubGateway</description>
      <source>[CONFIG_DIR]/sub-gateway/sub-gateway.c</source>
      <commands>$(MAKE) -j$(CPUS) sub-gateway.z1 TARGET=z1</commands>
      <firmware>[CONFIG_DIR]/sub-gateway/build/z1/sub-gateway.z1</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="50.69775325088584" y="44.23767367143136" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>2</id>
        </interface_config>
      </mote>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="112.7468734502921" y="23.669712244235992" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>6</id>
        </interface_config>
      </mote>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.Z1MoteType
      <description>LightSensor</description>
      <source>[CONFIG_DIR]/light-sensor/light-sensor.c</source>
      <commands>$(MAKE) -j$(CPUS) light-sensor.z1 TARGET=z1</commands>
      <firmware>[CONFIG_DIR]/light-sensor/build/z1/light-sensor.z1</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="21.463156291238107" y="68.38616121138223" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>3</id>
        </interface_config>
      </mote>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.Z1MoteType
      <description>Irrigation</description>
      <source>[CONFIG_DIR]/irrigation-system/irrigation-system.c</source>
      <commands>$(MAKE) -j$(CPUS) irrigation-system.z1 TARGET=z1</commands>
      <firmware>[CONFIG_DIR]/irrigation-system/build/z1/irrigation-system.z1</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="121.4627246985233" y="51.54510915391848" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>4</id>
        </interface_config>
      </mote>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="34.41014788446759" y="76.14608886719199" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>7</id>
        </interface_config>
      </mote>
    </motetype>
    <motetype>
      org.contikios.cooja.mspmote.Z1MoteType
      <description>Light</description>
      <source>[CONFIG_DIR]/light/light.c</source>
      <commands>$(MAKE) -j$(CPUS) light.z1 TARGET=z1</commands>
      <firmware>[CONFIG_DIR]/light/build/z1/light.z1</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDefaultSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="53.48715050013896" y="77.14176892125309" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>5</id>
        </interface_config>
      </mote>
      <mote>
        <interface_config>
          org.contikios.cooja.interfaces.Position
          <pos x="72.10859947000004" y="73.76715984486943" />
        </interface_config>
        <interface_config>
          org.contikios.cooja.mspmote.interfaces.MspMoteID
          <id>8</id>
        </interface_config>
      </mote>
    </motetype>
  </simulation>
  <plugin>
    org.contikios.cooja.plugins.Visualizer
    <plugin_config>
      <moterelations>true</moterelations>
      <skin>org.contikios.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.GridVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.TrafficVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.UDGMVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.MoteTypeVisualizerSkin</skin>
      <viewport>2.241255464341096 0.0 0.0 2.241255464341096 -18.190965141293187 19.420024823397007</viewport>
    </plugin_config>
    <bounds x="1" y="1" height="400" width="400" />
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.LogListener
    <plugin_config>
      <filter />
      <formatted_time />
      <coloring />
    </plugin_config>
    <bounds x="400" y="0" height="983" width="1212" z="1" />
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.MoteInterfaceViewer
    <mote_arg>6</mote_arg>
    <plugin_config>
      <interface>Leds</interface>
      <scrollpos>0,0</scrollpos>
    </plugin_config>
    <bounds x="25" y="410" height="300" width="350" z="3" />
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.MoteInterfaceViewer
    <mote_arg>7</mote_arg>
    <plugin_config>
      <interface>Leds</interface>
      <scrollpos>0,0</scrollpos>
    </plugin_config>
    <bounds x="26" y="713" height="300" width="350" z="2" />
  </plugin>
</simconf>
