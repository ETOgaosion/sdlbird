/*
 * Copyright (c) 2014, Wei Mingzhi <whistler_wmz@users.sf.net>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author and contributors may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "Sprite.h"
#include <search.h>
#include <assert.h>
#include <string.h>

CSprite::CSprite(SDL_Surface *pRenderer, const char *szImageFileName, const char *szTxtFileName)
{
  int ret = hcreate(512);
  assert(ret);
  Load(pRenderer, szImageFileName, szTxtFileName);
}

CSprite::~CSprite()
{
  if (m_pTexture != NULL)
    {
      SDL_FreeSurface(m_pTexture);
    }
  hdestroy();
}

/**
 * This hash function has been taken from an Article in Dr Dobbs Journal.
 * This is normally a collision-free function, distributing keys evenly.
 * Collision can be avoided by comparing the key itself in last resort.
 */
inline unsigned int CalcTag(const char *sz)
{
  unsigned int hash = 0;

  while (*sz)
    {
      hash += (unsigned short)*sz;
      hash += (hash << 10);
      hash ^= (hash >> 6);

      sz++;
    }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}

void CSprite::Draw(SDL_Surface *pRenderer, const char *szTag, int x, int y)
{
  ENTRY item;
  item.key = (char *)szTag;
  ENTRY *ret = hsearch(item, FIND);

  if (ret)
    {
      SDL_Rect srcrect, dstrect;
      SpritePart_t *it = (SpritePart_t *)ret->data;

      srcrect.x = it->X;
      srcrect.y = it->Y;
      srcrect.w = it->usWidth;
      srcrect.h = it->usHeight;

      dstrect.x = x;
      dstrect.y = y;
      dstrect.w = it->usWidth;
      dstrect.h = it->usHeight;

      SDL_BlitSurface(m_pTexture, &srcrect, pRenderer, &dstrect);
    }
}

void CSprite::DrawEx(SDL_Surface *pRenderer, const char *szTag, int x, int y, int angle)
{
  ENTRY item;
  item.key = (char *)szTag;
  ENTRY *ret = hsearch(item, FIND);

  if (ret)
    {
      SDL_Rect srcrect, dstrect;
      SpritePart_t *it = (SpritePart_t *)ret->data;

      srcrect.x = it->X;
      srcrect.y = it->Y;
      srcrect.w = it->usWidth;
      srcrect.h = it->usHeight;

      dstrect.x = x;
      dstrect.y = y;
      dstrect.w = it->usWidth;
      dstrect.h = it->usHeight;

      SDL_BlitSurface(m_pTexture, &srcrect, pRenderer, &dstrect);
    }
}

void CSprite::Load(SDL_Surface *pRenderer, const char *szImageFileName, const char *szTxtFileName)
{
  SDL_Surface *pSurface = IMG_Load(szImageFileName);

  if (pSurface == NULL)
    {
      fprintf(stderr, "CSprite::Load(): IMG_Load failed: %s\n", IMG_GetError());
      return;
    }

  m_iTextureWidth = pSurface->w;
  m_iTextureHeight = pSurface->h;

  m_pTexture = pSurface;

  // Load txt file
  if (!LoadTxt(szTxtFileName))
    {
      SDL_FreeSurface(m_pTexture);
      m_pTexture = NULL;

      fprintf(stderr, "CSprite::Load(): LoadTxte failed\n");
      return;
    }
}

bool CSprite::LoadTxt(const char *szTxtFileName)
{
  FILE *fp = fopen(szTxtFileName, "r");

  if (fp == NULL)
    {
      return false;
    }

  while (!feof(fp))
    {
      char name[256];
      int w, h, x, y;

      if (fscanf(fp, "%s %d %d %d %d", name, &w, &h, &x, &y) != 5)
	{
	  continue;
	}

      SpritePart_t *spritePart = new SpritePart_t;

      spritePart->usWidth = w;
      spritePart->usHeight = h;
      spritePart->X = x;
      spritePart->Y = y;

      ENTRY item;
      item.key = strdup(name);
      item.data = spritePart;
      ENTRY *ret = hsearch(item, ENTER);
      assert(ret != NULL);
    }

  fclose(fp);
  return true;
}
