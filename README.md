# PaintKitUtilsPlugin

## Summary
This is a clientside plugin for Team Fortress 2 that provides some commands that might be useful to War Paint authors, like dumping textures to disk or outputting keyvalues for a paintkit.

## Installation
Extract "PaintKitUtilsPlugin.dll" and "PaintKitUtilsPlugin.vdf" to your "tf/addons/" folder. Team Fortress 2 will only try to load this plugin if you set "-insecure" in your launch options to run without VAC.

## Usage
This addon introduces console commands, all of which start with "pku_" and include descriptions.

## Build
To build, you'll need to set your "PaintKitUtilsPlugin/SourceSDK2013.props" to point to your Source SDK "mp/src/" or other source folder. You'll also need to provide your own libprotobuf.lib.
