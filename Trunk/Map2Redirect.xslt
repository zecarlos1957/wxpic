<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- =============================================
 !  Purpose:
 !    Create one redirection file for the help
 !    for each anchor listed in the input map file
 !
 !  Parameter:
 !    TARGET_DIR: Absolute path to the directory
 !            where to store the redirection files
 !
 !  Author: 
 !    Philippe Chevrier  pch@laposte.net
 !
 !  License: see Doc/License.txt
 +============================================= -->
 
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:map="http://wxpic.free.fr/map" version="2.0">
	<xsl:output name="XHTML_FORMAT" method="xhtml" version="1.0" encoding="ISO-8859-1" indent="yes" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"/>
	<xsl:param name="TARGET_DIR" />
	<xsl:template match="/map:map">
		<!-- Remove leading and trailing '/' of path to avoid getting 2 in the final URI -->
		<xsl:variable name="CLEAN_TARGET_DIR" select="replace(replace(replace($TARGET_DIR,'\\','/'),'^/',''),'/$','')"/>
		<xsl:for-each select="map:entry[@anchor]">
			<xsl:variable name="URL_ANCHOR" select="replace(replace(@url,'(\.[^.]*)',concat('++',@anchor,'$1')),'\\','/')"/>
			<xsl:variable name="PATH_PART" select="tokenize(@url,'\\|/')"/>
			<xsl:variable name="RELATIVE_URL" select="concat('1;url=',$PATH_PART[count($PATH_PART)],'#',@anchor)" />
			<xsl:variable name="FILENAME" select="concat('file:/',$CLEAN_TARGET_DIR,'/',$URL_ANCHOR)"/>
			<xsl:result-document href="{$FILENAME}" format="XHTML_FORMAT">
				<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="FR" lang="FR">
					<head>
						<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
						<meta http-equiv="refresh" content="{$RELATIVE_URL}" />
						<title/>
					</head>
					<body>
					</body>
				</html>
			</xsl:result-document>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>