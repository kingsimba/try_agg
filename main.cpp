#include "agg2/agg_renderer_base.h"
#include "agg2/agg_pixfmt_gray.h"
#include "agg2/agg_conv_stroke.h"
#include <agg2/agg_rasterizer_scanline_aa.h>
#include <agg2/agg_renderer_scanline.h>
#include <agg2/agg_scanline_p.h>
#include "writepng_gray.h"

class SimplePath
{
  public:
    SimplePath(const double* points, int size) : m_points(points), m_size(size), m_pos(0) {}

    void rewind(int path = 0) { m_pos = 0; }

    unsigned vertex(double* x, double* y)
    {
        if (m_pos < m_size)
        {
            *x = m_points[m_pos++];
            *y = m_points[m_pos++];
            return (m_pos == 2 ? agg::path_cmd_move_to : agg::path_cmd_line_to);
        }
        return agg::path_cmd_stop;
    }

  private:
    const double* m_points;
    int m_size;
    int m_pos;
};

int main()
{
    int imageWidth = 64;
    int imageHeight = 64;

    typedef agg::pixfmt_gray8 PixelFormat;
    typedef agg::renderer_base<PixelFormat> RendererBaseType;
    const int pixelSize = PixelFormat::pix_width;
    agg::rendering_buffer renderBuffer;
    PixelFormat pixFmt(renderBuffer);
    RendererBaseType rBase;
    // Allocate a 100 x 100 rgba buffer
    unsigned char* imageBuffer = new unsigned char[imageWidth * imageHeight * pixelSize];
    // Hook everything up
    renderBuffer.attach(imageBuffer, imageWidth, imageHeight, imageWidth * pixelSize);
    pixFmt.attach(renderBuffer);
    rBase.attach(pixFmt);

    double border[] = {0, 0, 100, 100};

    SimplePath path(border, sizeof(border) / sizeof(double));

    agg::conv_stroke<SimplePath> strokePath(path);
    double strokeWidth = 1.0;
    strokePath.width(strokeWidth);
    strokePath.line_cap(agg::square_cap);
    strokePath.line_join(agg::miter_join);
    strokePath.miter_limit(strokeWidth);

    agg::rasterizer_scanline_aa<> ras;
    ras.add_path(strokePath);

    agg::scanline_p8 scanline;
    const agg::gray8 redColor(0x88);
    agg::render_scanlines_aa_solid(ras, scanline, rBase, redColor);

    // now write the image out for visualization
    writePng<RendererBaseType>("tutorial_linedrawing_1.png", rBase);

    return 0;
}