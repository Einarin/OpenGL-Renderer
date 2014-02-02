/*
 *      renderer.h
 *
 *      Copyright 2010 Johnny Dickinson kc0itv@gmail.com
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#pragma once
#include <string>

typedef int FlatAsset;

class FlatAssetManager
{
    public:
        //sets up screen
        virtual void setScreen(int screenWidth, int screenHeight);
        virtual void toggleFullscreen();
        virtual void renderColored(std::string name, std::string overlay, std::string colormask,int red, int green, int blue);
        virtual void completeBackgrounded();
        //requests an image resource
        virtual FlatAsset requestFileResource(std::string resource);
        //requests an image resource of text
        virtual void requestText(FlatAsset resource, int size, int red, int green, int blue);
        //releases an image resource
        virtual void releaseResource(std::string resource);
        //requests generation of resource at the given size
        virtual void cacheResource(std::string resource, int width, int height);
        //returns the native size of the given resource
        virtual std::pair<int,int> resourceSize(std::string resource);
        //draw() renders resources to the screen with various options, location is top left of image
        virtual void draw(FlatAsset resource, int x, int y, int width, int height, int angle);
        virtual void draw(FlatAsset resource, int x, int y, int width, int height);
        virtual void draw(FlatAsset resource, int x, int y);
        virtual void draw(FlatAsset resource, int x, int y, float scale);
        //drawCentered() acts like draw() but centers the resource at the given location
        virtual void drawCentered(FlatAsset resource, int x, int y, int width, int height, int angle);
        virtual void drawCentered(FlatAsset resource, int x, int y, int width, int height);
        virtual void drawCentered(FlatAsset resource, int x, int y);
        virtual void drawCentered(FlatAsset resource, int x, int y, float scale);
        virtual void updateScreen();
};
