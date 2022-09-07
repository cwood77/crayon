#include "snippet.hpp"
#include <stdexcept>

snippet::snippet(long w, long h)
: m_w(w), m_h(h)
{
   m_pBlock = new pixel [w*h];
}

snippet::~snippet()
{
   delete [] m_pBlock;
}

pixel& snippet::index(const point& p)
{
   if(p.x >= m_w || p.y >= m_h)
      throw std::runtime_error("point out of bounds");
   return m_pBlock[p.x + p.y*m_w];
}
