/*
MIT License

Copyright (c) 2021 lespalt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Overlay.h"
#include "Config.h"
#include "OverlayDebug.h"

class OverlayInputs : public Overlay
{
    public:

        OverlayInputs()
            : Overlay("OverlayInputs")
        {}

    protected:

        virtual float2 getDefaultSize()
        {
            return float2(400,100);
        }

        virtual void onConfigChanged()
        {
            // Width might have changed, reset tracker values
            m_throttleVtx.resize( m_width );
            m_brakeVtx.resize( m_width );
            m_steerVtx.resize( m_width );
            for( int i=0; i<m_width; ++i )
            {
                m_throttleVtx[i].x = float(i);
                m_brakeVtx[i].x = float(i);
                m_steerVtx[i].x = float(i);
            }
        }

        virtual void onUpdate()
        {
            const float w = (float)m_width;
            const float h = (float)m_height;

            // Make code below safe against indexing into size-1 when sizes are zero
            if( m_throttleVtx.empty() )
                m_throttleVtx.resize( 1 );
            if( m_brakeVtx.empty() )
                m_brakeVtx.resize( 1 );
            if( m_steerVtx.empty() )
                m_steerVtx.resize( 1 );

            // Advance input vertices
            for( int i=0; i<(int)m_throttleVtx.size()-1; ++i )
                m_throttleVtx[i].y = m_throttleVtx[i+1].y;
            m_throttleVtx[(int)m_throttleVtx.size()-1].y = ir_Throttle.getFloat();

            for( int i=0; i<(int)m_brakeVtx.size()-1; ++i )
                m_brakeVtx[i].y = m_brakeVtx[i+1].y;
            m_brakeVtx[(int)m_brakeVtx.size()-1].y = ir_Brake.getFloat();

            for( int i=0; i<(int)m_steerVtx.size()-1; ++i )
                m_steerVtx[i].y = m_steerVtx[i+1].y;
            m_steerVtx[(int)m_steerVtx.size()-1].y = std::min( 1.0f, std::max( 0.0f, (ir_SteeringWheelAngle.getFloat() / ir_SteeringWheelAngleMax.getFloat()) * -0.5f + 0.5f) );

            // Render
            const float thickness = g_cfg.getFloat( m_name, "line_thickness", 2.0f );
            auto vtx2coord = [&]( const float2& v )->float2 {
                return float2( v.x+0.5f, h-0.5f*thickness - v.y*(h-thickness) );
            };

            Microsoft::WRL::ComPtr<ID2D1PathGeometry1> throttleFillPath;
            Microsoft::WRL::ComPtr<ID2D1GeometrySink>  throttleFillSink;
            m_d2dFactory->CreatePathGeometry( &throttleFillPath );
            throttleFillPath->Open( &throttleFillSink );
            throttleFillSink->BeginFigure( float2(0,h), D2D1_FIGURE_BEGIN_FILLED );
            for( int i=0; i<(int)m_throttleVtx.size(); ++i )
                throttleFillSink->AddLine( vtx2coord(m_throttleVtx[i]) );
            throttleFillSink->AddLine( float2(m_throttleVtx[m_throttleVtx.size()-1].x+0.5f,h) );
            throttleFillSink->EndFigure( D2D1_FIGURE_END_OPEN );
            throttleFillSink->Close();

            Microsoft::WRL::ComPtr<ID2D1PathGeometry1> brakeFillPath;
            Microsoft::WRL::ComPtr<ID2D1GeometrySink>  brakeFillSink;
            m_d2dFactory->CreatePathGeometry( &brakeFillPath );
            brakeFillPath->Open( &brakeFillSink );
            brakeFillSink->BeginFigure( float2(0,h), D2D1_FIGURE_BEGIN_FILLED );
            for( int i=0; i<(int)m_brakeVtx.size(); ++i )
                brakeFillSink->AddLine( vtx2coord(m_brakeVtx[i]) );
            brakeFillSink->AddLine( float2(m_brakeVtx[m_brakeVtx.size()-1].x+0.5f,h) );
            brakeFillSink->EndFigure( D2D1_FIGURE_END_OPEN );
            brakeFillSink->Close();

            Microsoft::WRL::ComPtr<ID2D1PathGeometry1> throttleLinePath;
            Microsoft::WRL::ComPtr<ID2D1GeometrySink>  throttleLineSink;
            m_d2dFactory->CreatePathGeometry( &throttleLinePath );
            throttleLinePath->Open( &throttleLineSink );
            throttleLineSink->BeginFigure( vtx2coord(m_throttleVtx[0]), D2D1_FIGURE_BEGIN_HOLLOW );
            for( int i=1; i<(int)m_throttleVtx.size(); ++i )
                throttleLineSink->AddLine( vtx2coord(m_throttleVtx[i]) );
            throttleLineSink->EndFigure( D2D1_FIGURE_END_OPEN );
            throttleLineSink->Close();

            Microsoft::WRL::ComPtr<ID2D1PathGeometry1> brakeLinePath;
            Microsoft::WRL::ComPtr<ID2D1GeometrySink>  brakeLineSink;
            m_d2dFactory->CreatePathGeometry( &brakeLinePath );
            brakeLinePath->Open( &brakeLineSink );
            brakeLineSink->BeginFigure( vtx2coord(m_brakeVtx[0]), D2D1_FIGURE_BEGIN_HOLLOW );
            for( int i=1; i<(int)m_brakeVtx.size(); ++i )
                brakeLineSink->AddLine( vtx2coord(m_brakeVtx[i]) );
            brakeLineSink->EndFigure( D2D1_FIGURE_END_OPEN );
            brakeLineSink->Close();

            Microsoft::WRL::ComPtr<ID2D1PathGeometry1> steeringLinePath;
            Microsoft::WRL::ComPtr<ID2D1GeometrySink>  steeringLineSink;
            m_d2dFactory->CreatePathGeometry( &steeringLinePath );
            steeringLinePath->Open( &steeringLineSink );
            steeringLineSink->BeginFigure( vtx2coord(m_steerVtx[0]), D2D1_FIGURE_BEGIN_HOLLOW );
            for( int i=1; i<(int)m_steerVtx.size(); ++i )
                steeringLineSink->AddLine( vtx2coord(m_steerVtx[i]) );
            steeringLineSink->EndFigure( D2D1_FIGURE_END_OPEN );
            steeringLineSink->Close();

            m_renderTarget->BeginDraw();
            m_brush->SetColor( g_cfg.getFloat4( m_name, "throttle_fill_col", float4(0.2f,0.45f,0.15f,0.6f) ) );
            m_renderTarget->FillGeometry( throttleFillPath.Get(), m_brush.Get() );
            m_brush->SetColor( g_cfg.getFloat4( m_name, "brake_fill_col", float4(0.46f,0.01f,0.06f,0.6f) ) );
            m_renderTarget->FillGeometry( brakeFillPath.Get(), m_brush.Get() );
            m_brush->SetColor( g_cfg.getFloat4( m_name, "throttle_col", float4(0.38f,0.91f,0.31f,0.8f) ) );
            m_renderTarget->DrawGeometry( throttleLinePath.Get(), m_brush.Get(), thickness );
            m_brush->SetColor( g_cfg.getFloat4( m_name, "brake_col", float4(0.93f,0.03f,0.13f,0.8f) ) );
            m_renderTarget->DrawGeometry( brakeLinePath.Get(), m_brush.Get(), thickness );
            m_brush->SetColor( g_cfg.getFloat4( m_name, "steering_col", float4(1,1,1,0.3f) ) );
            m_renderTarget->DrawGeometry( steeringLinePath.Get(), m_brush.Get(), thickness );
            m_renderTarget->EndDraw();
        }

    protected:

        std::vector<float2> m_throttleVtx;
        std::vector<float2> m_brakeVtx;
        std::vector<float2> m_steerVtx;
};
