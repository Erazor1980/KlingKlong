/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight,16u ) );
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

RectF Graphics::GetScreenRect()
{
    return { 0.0f, float( ScreenWidth ), 0.0f, float( ScreenHeight ) };
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(),L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
		}
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

void Graphics::DrawRect( int x0,int y0,int x1,int y1,Color c )
{
	if( x0 > x1 )
	{
		std::swap( x0,x1 );
	}
	if( y0 > y1 )
	{
		std::swap( y0,y1 );
	}

	for( int y = y0; y < y1; ++y )
	{
		for( int x = x0; x < x1; ++x )
		{
			PutPixel( x,y,c );
		}
	}
}

void Graphics::DrawRectBorder( const RectF& rect, const int border, Color c )
{
    const Vec2 topLeft( rect.left, rect.top );
    const Vec2 bottomRight( rect.right, rect.bottom );

    // top line
    DrawRect( ( int )topLeft.x, ( int )topLeft.y, ( int )bottomRight.x, ( int )topLeft.y + border, c );
    // bottom line
    DrawRect( ( int )topLeft.x, ( int )bottomRight.y - border, ( int )bottomRight.x, ( int )bottomRight.y, c );
    // left line
    DrawRect( ( int )topLeft.x, ( int )topLeft.y, ( int )topLeft.x + border, ( int )bottomRight.y, c );
    // right line
    DrawRect( ( int )bottomRight.x - border, ( int )topLeft.y, ( int )bottomRight.x, ( int )bottomRight.y, c );
}

void Graphics::DrawCircle( int x,int y,int radius,Color c )
{
	const int rad_sq = radius * radius;
	for( int y_loop = y - radius + 1; y_loop < y + radius; y_loop++ )
	{		
		for( int x_loop = x - radius + 1; x_loop < x + radius; x_loop++ )
		{
			const int x_diff = x - x_loop;
			const int y_diff = y - y_loop;
			if( x_diff * x_diff + y_diff * y_diff <= rad_sq )
			{
				PutPixel( x_loop,y_loop,c );
			}
		}
	}
}

void Graphics::DrawIsoRightTriUL( int x, int y, int size, Color c )
{
    for( int loop_y = y; loop_y < y + size; loop_y++ )
    {
        const int cur_line = loop_y - y;
        for( int loop_x = x; loop_x < x + size - cur_line; loop_x++ )
        {
            PutPixel( loop_x, loop_y, c );
        }
    }
}

void Graphics::DrawIsoRightTriUR( int x, int y, int size, Color c )
{
    for( int loop_y = y; loop_y < y + size; loop_y++ )
    {
        const int cur_line = loop_y - y;
        for( int loop_x = x + cur_line; loop_x < x + size; loop_x++ )
        {
            PutPixel( loop_x, loop_y, c );
        }
    }
}

void Graphics::DrawIsoRightTriBL( int x, int y, int size, Color c )
{
    for( int loop_y = y; loop_y < y + size; loop_y++ )
    {
        const int cur_line = loop_y - y;
        for( int loop_x = x; loop_x < x + cur_line; loop_x++ )
        {
            PutPixel( loop_x, loop_y, c );
        }
    }
}

void Graphics::DrawIsoRightTriBR( int x, int y, int size, Color c )
{
    for( int loop_y = y; loop_y < y + size; loop_y++ )
    {
        const int cur_line = loop_y - y;
        for( int loop_x = x + size - cur_line; loop_x < x + size; loop_x++ )
        {
            PutPixel( loop_x, loop_y, c );
        }
    }
}

void Graphics::DrawSprite( int x, int y, const Surface& src )
{
    // src rect
    int src_left = 0;
    int src_right = src.GetWidth();
    int src_top = 0;
    int src_bottom = src.GetHeight();
    // dst rect
    int dst_left = x;
    int dst_right = x + src_right;
    int dst_top = y;
    int dst_bottom = y + src_bottom;
    // clip to screen (left/right)
    if( dst_left < 0 )
    {
        const int diff = -dst_left;
        src_left += diff;
        dst_left += diff;
    }
    if( dst_right > Graphics::ScreenWidth )
    {
        const int diff = dst_right - Graphics::ScreenWidth;
        src_right -= diff;
        dst_right -= diff;
    }
    // early rejection test (left/right)
    if( dst_left >= dst_right )
    {
        return;
    }
    // clip to screen (top/bottom)
    if( dst_top < 0 )
    {
        const int diff = -dst_top;
        src_top += diff;
        dst_top += diff;
    }
    if( dst_bottom > Graphics::ScreenHeight )
    {
        const int diff = dst_bottom - Graphics::ScreenHeight;
        src_bottom -= diff;
        dst_bottom -= diff;
    }
    // copy the clipped sprite pixel block
    for( int y_src = src_top, y_dst = dst_top;
         y_src < src_bottom; y_src++, y_dst++ )
    {
        for( int x_src = src_left, x_dst = dst_left;
             x_src < src_right; x_src++, x_dst++ )
        {
            PutPixel( x_dst, y_dst, src.GetPixel( x_src, y_src ) );
        }
    }
}

void Graphics::DrawSpriteKey( int x, int y, const Surface &src, Color key )
{
    // src rect
    int src_left = 0;
    int src_right = src.GetWidth();
    int src_top = 0;
    int src_bottom = src.GetHeight();
    // dst rect
    int dst_left = x;
    int dst_right = x + src_right;
    int dst_top = y;
    int dst_bottom = y + src_bottom;
    // clip to screen (left/right)
    if( dst_left < 0 )
    {
        const int diff = -dst_left;
        src_left += diff;
        dst_left += diff;
    }
    if( dst_right > Graphics::ScreenWidth )
    {
        const int diff = dst_right - Graphics::ScreenWidth;
        src_right -= diff;
        dst_right -= diff;
    }
    // early rejection test (left/right)
    if( dst_left >= dst_right )
    {
        return;
    }
    // clip to screen (top/bottom)
    if( dst_top < 0 )
    {
        const int diff = -dst_top;
        src_top += diff;
        dst_top += diff;
    }
    if( dst_bottom > Graphics::ScreenHeight )
    {
        const int diff = dst_bottom - Graphics::ScreenHeight;
        src_bottom -= diff;
        dst_bottom -= diff;
    }
    // copy the clipped sprite pixel block
    for( int y_src = src_top, y_dst = dst_top;
         y_src < src_bottom; y_src++, y_dst++ )
    {
        for( int x_src = src_left, x_dst = dst_left;
             x_src < src_right; x_src++, x_dst++ )
        {
            const Color srcPixel = src.GetPixel( x_src, y_src );
            if( srcPixel != key )
            {
                PutPixel( x_dst, y_dst, srcPixel );
            }
        }
    }
}

void Graphics::DrawString( const char* text, int x, int y, Font& font, const Surface& fontSurf, Color color )
{
    for( int i = 0; text[ i ] != '\0'; ++i )
    {
        DrawChar( text[ i ], x + i * font.charWidth, y, font, fontSurf, color );
    }
}
void Graphics::DrawChar( char c, int xOff, int yOff, Font& font, const Surface& fontSurf, Color color )
{
    if( c < ' ' || c > '~' ) return;

    if( xOff + font.charWidth > ScreenWidth - 1 || yOff + font.charHeight > ScreenHeight - 1 )
        return;

    const int sheetIndex = c - ' ';
    const int sheetCol = sheetIndex % font.nCharsPerRow;
    const int sheetRow = sheetIndex / font.nCharsPerRow;
    const int xStart = sheetCol * font.charWidth;
    const int yStart = sheetRow * font.charHeight;
    const int xEnd = xStart + font.charWidth;
    const int yEnd = yStart + font.charHeight;
    const int surfWidth = font.charWidth * font.nCharsPerRow;

    //DrawSpriteKeyFromSequence( xOff, yOff, font.surface, font.surface.GetPixel( 0, 0 ), sheetIndex, 32, 3 );
    for( int y = yStart; y < yEnd; ++y )
    {
        for( int x = xStart; x < xEnd; ++x )
        {
            if( fontSurf.GetPixel( x, y ).GetB() == 0 ) //Colors::Black <- didnt work! dont know why... all channels are 0
            {
                if( x >= 0 && y >= 0 && x < ScreenWidth && y < ScreenHeight )
                {
                    PutPixel( xOff + x - xStart, yOff + y - yStart, color );
                }
            }
        }
    }
}

void Graphics::DrawSpriteKeyFromSequence( int x, int y, const Surface& src, Color key, unsigned int idx, unsigned int imagesPerRow, unsigned int imagesPerColumn )
{
    // some checks
    assert( idx < imagesPerRow * imagesPerColumn );
    const int subImgWidth = src.GetWidth() / imagesPerRow;
    const int subImgHeight = src.GetHeight() / imagesPerColumn;
    assert( src.GetWidth() % subImgWidth == 0 );
    assert( src.GetHeight() % subImgHeight== 0 );

    // src rect
    int src_left = ( idx % imagesPerRow ) * subImgWidth;
    int src_right = src_left + subImgWidth;
    int src_top = idx / imagesPerRow * subImgHeight;
    int src_bottom = src_top + subImgHeight;
    // dst rect
    int dst_left = x;
    int dst_right = x + subImgWidth;
    int dst_top = y;
    int dst_bottom = y + subImgHeight;
    // clip to screen (left/right)
    if( dst_left < 0 )
    {
        const int diff = -dst_left;
        src_left += diff;
        dst_left += diff;
    }
    if( dst_right > Graphics::ScreenWidth )
    {
        const int diff = dst_right - Graphics::ScreenWidth;
        src_right -= diff;
        dst_right -= diff;
    }
    // early rejection test (left/right)
    if( dst_left >= dst_right )
    {
        return;
    }
    // clip to screen (top/bottom)
    if( dst_top < 0 )
    {
        const int diff = -dst_top;
        src_top += diff;
        dst_top += diff;
    }
    if( dst_bottom > Graphics::ScreenHeight )
    {
        const int diff = dst_bottom - Graphics::ScreenHeight;
        src_bottom -= diff;
        dst_bottom -= diff;
    }
    // copy the clipped sprite pixel block
    for( int y_src = src_top, y_dst = dst_top;
         y_src < src_bottom; y_src++, y_dst++ )
    {
        for( int x_src = src_left, x_dst = dst_left;
             x_src < src_right; x_src++, x_dst++ )
        {
            const Color srcPixel = src.GetPixel( x_src, y_src );
            if( srcPixel != key )
            {
                PutPixel( x_dst, y_dst, srcPixel );
            }
        }
    }
}

//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}