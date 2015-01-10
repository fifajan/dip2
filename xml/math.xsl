<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">

  <xsl:variable name="eps">0.0000000000000001</xsl:variable>
	<xsl:variable name="pi">3.141592653589793</xsl:variable>
	<xsl:template name="atan">
		<xsl:param name="x"/>
		<xsl:param name="k" select="3"/>
		<xsl:param name="y" select="$x * $x"/>
		<xsl:param name="i" select="-1"/>
		<xsl:param name="u" select="$x * $y"/>
		<xsl:choose>
			<xsl:when test="$u &gt; $eps">
				<xsl:call-template name="atan">
					<xsl:with-param name="x" select="$x + $u * $i div $k"/>
					<xsl:with-param name="k" select="$k + 2"/>
					<xsl:with-param name="y" select="$y"/>
					<xsl:with-param name="u" select="$u * $y"/>
					<xsl:with-param name="i" select="-$i"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$x"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="sqrt">
		<xsl:param name="x"/>
		<xsl:param name="u1" select="$x"/>
		<xsl:param name="u2" select="1"/>
		<xsl:choose>
			<xsl:when test="$x &lt; 0">NaN</xsl:when>
			<xsl:when test="(($u2 - $u1)*(number($u2 &gt; $u1) * 2 - 1)) &gt; $eps">
				<xsl:call-template name="sqrt">
					<xsl:with-param name="x" select="$x"/>
					<xsl:with-param name="u1" select="$u2"/>
					<xsl:with-param name="u2" select="($u2 + $x div $u2) div 2"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$u2"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="k">
		<xsl:param name="x"/>
		<xsl:param name="k"/>
		<xsl:choose>
			<xsl:when test="$k &gt;= $x">
				<xsl:call-template name="k">
					<xsl:with-param name="x" select="$x"/>
					<xsl:with-param name="k" select="$k div 2"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$k"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="sincostan">
		<xsl:param name="res"/>
		<xsl:param name="x"/>
		<xsl:param name="k" select="0.5"/>
		<xsl:param name="xx" select="1"/>
		<xsl:param name="yy" select="0"/>
		<xsl:choose>
			<xsl:when test="$x &gt; $eps">
				<xsl:variable name="k1">
					<xsl:call-template name="k">
						<xsl:with-param name="x" select="$x"/>
						<xsl:with-param name="k" select="$k"/>
					</xsl:call-template>
				</xsl:variable>
				<xsl:variable name="x1"><xsl:call-template name="atan"><xsl:with-param name="x" select="$k1"/></xsl:call-template></xsl:variable>
				<xsl:variable name="q"><xsl:call-template name="sqrt"><xsl:with-param name="x" select="1 + $k1 * $k1"/></xsl:call-template></xsl:variable>
				<xsl:call-template name="sincostan">
					<xsl:with-param name="res" select="$res"/>
					<xsl:with-param name="x" select="$x - $x1"/>
					<xsl:with-param name="k" select="$k1"/>
					<xsl:with-param name="xx" select="($xx - $yy * $k1) div $q"/>
					<xsl:with-param name="yy" select="($xx * $k1 + $yy) div $q"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="$res = 'sin'"><xsl:value-of select="$yy"/></xsl:when>
					<xsl:when test="$res = 'cos'"><xsl:value-of select="$xx"/></xsl:when>
					<xsl:when test="$res = 'tan'"><xsl:value-of select="$yy div $xx"/></xsl:when>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="cos">
		<xsl:param name="x"/>
		<xsl:variable name="x0" select="($x div $pi div 2 - floor($x div $pi div 2)) * $pi * 2"/>
		<xsl:variable name="x1">
			<xsl:choose>
				<xsl:when test="$x0 = 0 or $x0 = $pi">0</xsl:when>
				<xsl:when test="$x0 &gt; $pi div 2 and $x0 &lt; $pi"><xsl:value-of select="$x0 - $pi"/></xsl:when>
				<xsl:when test="$x0 &gt; $pi and $x0 &lt; $pi * 1.5"><xsl:value-of select="$pi - $x0"/></xsl:when>
				<xsl:when test="$x0 &gt; $pi"><xsl:value-of select="$pi * 2 - $x0"/></xsl:when>
				<xsl:otherwise><xsl:value-of select="$x0"/></xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$x1 = 0">1</xsl:when>
			<xsl:when test="$x1 = $pi div 2">0</xsl:when>
			<xsl:when test="$x1 = $pi">-1</xsl:when>
			<xsl:when test="$x1 = $pi * 1.5">0</xsl:when>
			<xsl:when test="$x1 &gt; 0">
				<xsl:call-template name="sincostan">
					<xsl:with-param name="res">cos</xsl:with-param>
					<xsl:with-param name="x" select="$x1"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>-</xsl:text>
				<xsl:call-template name="sincostan">
					<xsl:with-param name="res">cos</xsl:with-param>
					<xsl:with-param name="x" select="-$x1"/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="sin">
		<xsl:param name="x"/>
		<xsl:variable name="x0" select="($x div $pi div 2 - floor($x div $pi div 2)) * $pi * 2"/>
		<xsl:variable name="x1">
			<xsl:choose>
				<xsl:when test="$x0 = 0 or $x0 = $pi">0</xsl:when>
				<xsl:when test="$x0 &gt; $pi div 2 and $x0 &lt; $pi * 1.5"><xsl:value-of select="$pi - $x0"/></xsl:when>
				<xsl:when test="$x0 &gt; $pi * 1.5"><xsl:value-of select="$x0 - $pi * 2"/></xsl:when>
				<xsl:otherwise><xsl:value-of select="$x0"/></xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$x1 = 0">0</xsl:when>
			<xsl:when test="$x1 = $pi div 2">1</xsl:when>
			<xsl:when test="$x1 = $pi">0</xsl:when>
			<xsl:when test="$x1 = $pi * 1.5">-1</xsl:when>
			<xsl:when test="$x1 &gt; 0">
				<xsl:call-template name="sincostan">
					<xsl:with-param name="res">sin</xsl:with-param>
					<xsl:with-param name="x" select="$x1"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>-</xsl:text>
				<xsl:call-template name="sincostan">
					<xsl:with-param name="res">sin</xsl:with-param>
					<xsl:with-param name="x" select="-$x1"/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="log">
		<xsl:param name="x"/>
		<xsl:choose>
			<xsl:when test="$x &lt;=0">NaN</xsl:when>
			<xsl:when test="$x = 1">0</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="x0">
					<xsl:choose>
						<xsl:when test="$x &lt; 1"><xsl:value-of select="1 div $x"/></xsl:when>
						<xsl:otherwise><xsl:value-of select="$x"/></xsl:otherwise>
					</xsl:choose>
				</xsl:variable>
				<xsl:variable name="x1">
					<xsl:call-template name="log_">
						<xsl:with-param name="x" select="concat(substring(string($x0),1,1),'.',translate(substring(string($x0),2),'.',''))"/>
						<xsl:with-param name="r" select="string-length(substring-before(concat(string($x0),'.'),'.')) * 2.30258509299404568402"/>
						<xsl:with-param name="log_numbers">2,1.1,1.01,1.001,1.0001,1.00001,1.000001,1.0000001,1.00000001,</xsl:with-param>
						<xsl:with-param name="log_values">0.69314718055994530942,0.09531017980432486004,0.00995033085316808285,0.00099950033308353317,0.00009999500033308335,0.00000999995000033333,0.00000099999950000033,0.00000009999999500000,0.00000000999999995000,</xsl:with-param>
					</xsl:call-template>
				</xsl:variable>
				<xsl:value-of select="$x1 * (number($x = $x0) * 2 - 1)"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="log_">
		<xsl:param name="x"/>
		<xsl:param name="r"/>
		<xsl:param name="log_numbers"/>
		<xsl:param name="log_values"/>
		<xsl:choose>
			<xsl:when test="$log_numbers != ''">
				<xsl:variable name="temp" select="number(substring-before($log_numbers,',')) * $x"/>
				<xsl:choose>
					<xsl:when test="$temp &lt; 10">
						<xsl:call-template name="log_">
							<xsl:with-param name="x" select="$temp"/>
							<xsl:with-param name="r" select="$r - number(substring-before($log_values,','))"/>
							<xsl:with-param name="log_numbers" select="$log_numbers"/>
							<xsl:with-param name="log_values" select="$log_values"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="log_">
							<xsl:with-param name="x" select="$x"/>
							<xsl:with-param name="r" select="$r"/>
							<xsl:with-param name="log_numbers" select="substring-after($log_numbers,',')"/>
							<xsl:with-param name="log_values" select="substring-after($log_values,',')"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise><xsl:value-of select="$r - (10 - $x) div 10"/></xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>