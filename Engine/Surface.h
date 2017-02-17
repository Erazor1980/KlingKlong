/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Surface.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "ChiliWin.h"
#include "Colors.h"
#include "ChiliException.h"
#include <string>
#include <assert.h>
#include <memory>

class Surface
{
public:
    class Exception : public ChiliException
    {
    public:
        using ChiliException::ChiliException;
        virtual std::wstring GetFullMessage() const override { return GetNote() + L"\nAt: " + GetLocation(); }
        virtual std::wstring GetExceptionType() const override { return L"Surface Exception"; }
    };
public:
    Surface( unsigned int width, unsigned int height, unsigned int pitch )
        :
        pBuffer( std::make_unique<Color[]>( width * height ) ),
        width( width ),
        height( height )
    {
    }
    Surface( Surface&& source )
        :
        pBuffer( std::move( source.pBuffer ) ),
        width( source.width ),
        height( source.height )
    {
    }
    Surface( Surface& ) = delete;
    Surface& operator=( Surface&& donor )
    {
        width = donor.width;
        height = donor.height;
        pBuffer = std::move( donor.pBuffer );
        donor.pBuffer = nullptr;
        return *this;
    }
    Surface& operator=( const Surface& ) = delete;
    ~Surface()
    {
    }
    void Clear( Color fillValue )
    {
        std::fill( GetBufferPtr(), GetBufferPtr() + width * height, fillValue );
    }
    void PutPixel( unsigned int x, unsigned int y, Color c )
    {
        assert( x >= 0 );
        assert( y >= 0 );
        assert( x < width );
        assert( y < height );
        pBuffer[ y * width + x ] = c;
    }
    void PutPixelAlpha( unsigned int x, unsigned int y, Color c );
    Color GetPixel( unsigned int x, unsigned int y ) const
    {
        assert( x >= 0 );
        assert( y >= 0 );
        assert( x < width );
        assert( y < height );
        return pBuffer[ y * width + x ];
    }
    unsigned int GetWidth() const
    {
        return width;
    }
    unsigned int GetHeight() const
    {
        return height;
    }
    Color* GetBufferPtr()
    {
        return pBuffer.get();
    }
    const Color* GetBufferPtrConst() const
    {
        return pBuffer.get();
    }
    static Surface FromFile( const std::wstring& name );
    void Save( const std::wstring& filename ) const;
    void Copy( const Surface& src );
private:
    Surface( unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam )
        :
        width( width ),
        height( height ),
        pBuffer( std::move( pBufferParam ) )
    {
    }
private:
    std::unique_ptr<Color[]> pBuffer;
    unsigned int width;
    unsigned int height;
};