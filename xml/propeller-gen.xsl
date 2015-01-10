<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">

  <xsl:param name="yzDim" select="80"/>
  <xsl:param name="bladeCount" select="5"/>
  <xsl:param name="bladeTilt" select="0.55"/>
  <xsl:param name="axisRadius" select="1.5"/>
  <!--
  <xsl:param name="bladeLength" select="'void'"/>
  <xsl:param name="bladeWidth" select="'void'"/>
  <xsl:param name="axisRadius" select="1.3"/>
  -->
  
  <xsl:variable name="pi">3.141592653589793</xsl:variable>
  <xsl:variable name="angleStep" select="2.0 * $pi div number($bladeCount)"/>
  
  <xsl:template name="makeBlades">
    <xsl:param name="count"/>
    <xsl:param name="angle"/>
    <xsl:if test="$count > 0">
    
      <meta-object type="expression" value="R^N/((kX*(cos(a)*x-sin(a)*(cos(b)*y-sin(b)*z))-dX)^N+(kY*(sin(a)*x+cos(a)*(cos(b)*y-sin(b)*z))-dY)^N+(kZ*(sin(b)*y+cos(b)*z)-dZ)^N)">
          <var name="N" value="4.000000"/>
          <var name="R" value="7.000000"/>
          
          <xsl:element name="var">
            <xsl:attribute name="name" select="'a'"/>
            <xsl:attribute name="value" select="number($bladeTilt)"/>
          </xsl:element>
          
          <xsl:element name="var">
            <xsl:attribute name="name" select="'b'"/>
            <xsl:attribute name="value" select="$angle"/>
          </xsl:element>
          
          <var name="dX" value="0.000000"/>
          <var name="dY" value="0.000000"/>
          <var name="dZ" value="11.500000"/>
          <var name="kX" value="1.000000"/>
          <var name="kY" value="7.000000"/>
          <var name="kZ" value="0.50000"/>
      </meta-object>
      
      <xsl:call-template name="makeBlades">
        <xsl:with-param name="count" select="$count - 1"/>
        <xsl:with-param name="angle" select="$angle + $angleStep"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>
  
  <xsl:template name="propllerGenerator">
  
    <document type="meta-objects" name="not-used">
      <view>
      
          <xsl:element name="grid">
            <xsl:attribute name="dimX" select="40"/>
            <xsl:attribute name="dimY" select="$yzDim"/>
            <xsl:attribute name="dimZ" select="$yzDim"/>
            <xsl:attribute name="isoMult" select="0.2"/>
            <xsl:attribute name="iso" select="3"/>
          </xsl:element>

          <camera rotX="9" rotY="0" rotZ="0" scaleMult="0.1900" scale="3"/>
          <display axises="false" polygonMode="faces" normalMode="smooth"/>
          <colors background="#0f0f0f" light1="#ffff00" light2="#bbbb00"/>
      </view>
      <field>
    
        <xsl:call-template name="makeBlades">
          <xsl:with-param name="count" select="number($bladeCount)"/>
          <xsl:with-param name="angle" select="0.0"/>
        </xsl:call-template>
        
        <meta-object type="expression" value="R/sqrt((z-dZ)^2.000+(y-dY)^2.000)">
        
          <xsl:element name="var">
            <xsl:attribute name="name" select="'R'"/>
            <xsl:attribute name="value" select="number($axisRadius)"/>
          </xsl:element>
          
          <var name="dY" value="0.000000"></var>
          <var name="dZ" value="0.000000"></var>
        </meta-object>
        
      </field>
    </document>
    
  </xsl:template>

</xsl:stylesheet>