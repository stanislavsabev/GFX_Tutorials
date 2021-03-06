#include "SpriteBatch.h"
#include "ErrManager.h"
#include <algorithm>

namespace ge {

	Glyph::Glyph(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint m_texture, float depth, const ColorRGBA8 & color) :
		m_texture(m_texture),
		depth(depth)
	{
		topLeft.color = color;
		topLeft.setPos(destRect.x, destRect.y + destRect.w);
		topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPos(destRect.x, destRect.y);
		bottomLeft.setUV(uvRect.x, uvRect.y);

		bottomRight.color = color;
		bottomRight.setPos(destRect.x + destRect.z, destRect.y);
		bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

		topRight.color = color;
		topRight.setPos(destRect.x + destRect.z, destRect.y + destRect.w);
		topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
	}

	Glyph::Glyph(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint m_texture, float depth, const ColorRGBA8 & color, float angle) :
		m_texture(m_texture),
		depth(depth)
	{
		glm::vec2 halfDims(destRect.z / 2.0f, destRect.w / 2.0f);

		// Get points centered at origin
		glm::vec2 tl(-halfDims.x, halfDims.y);
		glm::vec2 bl(-halfDims.x, -halfDims.y);
		glm::vec2 br(halfDims.x, -halfDims.y);
		glm::vec2 tr(halfDims.x, halfDims.y);

		// Rotate the points
		tl = rotatePoint(tl, angle) + halfDims;
		bl = rotatePoint(bl, angle) + halfDims;
		br = rotatePoint(br, angle) + halfDims;
		tr = rotatePoint(tr, angle) + halfDims;

		topLeft.color = color;
		topLeft.setPos(destRect.x + tl.x, destRect.y + tl.y);
		topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPos(destRect.x + bl.x, destRect.y + bl.y);
		bottomLeft.setUV(uvRect.x, uvRect.y);

		bottomRight.color = color;
		bottomRight.setPos(destRect.x + br.x, destRect.y + br.y);
		bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

		topRight.color = color;
		topRight.setPos(destRect.x + tr.x, destRect.y + tr.y);
		topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
	}

	glm::vec2 Glyph::rotatePoint(const glm::vec2& pos, float angle) {
		glm::vec2 newVec;
		newVec.x = pos.x * cos(angle) - pos.y * sin(angle);
		newVec.y = pos.x * sin(angle) + pos.y * cos(angle);
		return newVec;
	}

	/// <summary>
	///  To be able to draw bathces of textures
	/// </summary>
	SpriteBatch::SpriteBatch() : vbo(0), vao(0) {}

	SpriteBatch::~SpriteBatch() { }

	void SpriteBatch::init()
	{
		createVertexArray();
	}

	void SpriteBatch::begin(GlyphSortType sortBy /* GlyphSortType::TEXTURE */)
	{ /// to setup any state before rendering

		// how to sort the Glyphs
		sortType = sortBy;
		renderBatches.clear();
		glyphs.clear();
	}

	void SpriteBatch::end()
	{
		// resize and retarget
		glyphPtrs.resize(glyphs.size());
		for (size_t i = 0; i < glyphPtrs.size(); i++) {
			glyphPtrs[i] = &glyphs[i];
		}

		sortGlyphs();
		createRenderBatches();
	}

	// TODO
	void SpriteBatch::draw(const glm::vec4 & destRect,
		const glm::vec4 & uvRect, GLuint m_texture, float depth, const ColorRGBA8 & color)
	{
		glyphs.emplace_back(destRect, uvRect, m_texture, depth, color);
	}

	void SpriteBatch::draw(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint m_texture, float depth, const ColorRGBA8 & color, float angle)
	{
		glyphs.emplace_back(destRect, uvRect, m_texture, depth, color, angle);
	}

	void SpriteBatch::draw(const glm::vec4 & destRect, const glm::vec4 & uvRect, GLuint m_texture, float depth, const ColorRGBA8 & color, const glm::vec2& dir)
	{
		// determining angle from direction
		const glm::vec2 right(1.f, 0.f);
		float angle = acos(glm::dot(right, dir));
		if (dir.y < 0.f)  angle = -angle;

		glyphs.emplace_back(destRect, uvRect, m_texture, depth, color, angle);
	}

	void SpriteBatch::renderBatch()
	{
		// binding vertex array object
		GLCall(glBindVertexArray(vao));

		// drawing each batch
		for (size_t i = 0; i < renderBatches.size(); i++) {
			GLCall(glBindTexture(GL_TEXTURE_2D, renderBatches[i].m_texture));

			GLCall(glDrawArrays(GL_TRIANGLES, renderBatches[i].offset, renderBatches[i].numVertices));
		}

		// unbinding the array objects
		GLCall(glBindVertexArray(0));
	}

	void SpriteBatch::createRenderBatches()
	{
		if (glyphs.empty()) { return; }

		// vertex array to be uploaded to vbo
		// alocating all the memory we need for all
		// vertices for the glyphs
		// this means it can be treated as an array
		// and this is a lot faster than push back new item every time
		std::vector <Vertex> vertices;
		vertices.resize(glyphs.size() * 6);

		int offset = 0;
		int cv = 0; // current vertex

		// storing the first batch
		renderBatches.emplace_back(offset, 6, glyphPtrs[0]->m_texture);
		offset += 6;

		vertices[cv++] = glyphPtrs[0]->topLeft;
		vertices[cv++] = glyphPtrs[0]->bottomLeft;
		vertices[cv++] = glyphPtrs[0]->bottomRight;
		vertices[cv++] = glyphPtrs[0]->bottomRight;
		vertices[cv++] = glyphPtrs[0]->topRight;
		vertices[cv++] = glyphPtrs[0]->topLeft;

		// setting the rest of the batches
		for (size_t cg = 1; cg < glyphPtrs.size(); cg++) {
			if (glyphPtrs[cg]->m_texture != glyphPtrs[cg - 1]->m_texture) {
				renderBatches.emplace_back(offset, 6, glyphPtrs[cg]->m_texture);
			}
			else {
				renderBatches.back().numVertices += 6;
			}
			vertices[cv++] = glyphPtrs[cg]->topLeft;
			vertices[cv++] = glyphPtrs[cg]->bottomLeft;
			vertices[cv++] = glyphPtrs[cg]->bottomRight;
			vertices[cv++] = glyphPtrs[cg]->bottomRight;
			vertices[cv++] = glyphPtrs[cg]->topRight;
			vertices[cv++] = glyphPtrs[cg]->topLeft;
			offset += 6;
		}

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

		// orphan the buffer
		GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));
		// uploading the data
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));
		// unbinding the buffer
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	void SpriteBatch::createVertexArray()
	{
		if (0 == vao) {
			GLCall(glGenVertexArrays(1, &vao));
		}
		GLCall(glBindVertexArray(vao));

		if (0 == vbo) {
			GLCall(glGenBuffers(1, &vbo));
		}
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

		// Telling OpenGL what kind of attributes we're sending
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glEnableVertexAttribArray(2));

		// Passing more info (for flexibility if we don't want to draw the entire array)
		// This is the position attribute pointer 
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void *)offsetof(Vertex, pos)));

		// This is the color attribute pointer 
		GLCall(glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
			sizeof(Vertex), (void *)offsetof(Vertex, color)));

		// this is the UV attribute pointer
		GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void *)offsetof(Vertex, uv)));

		// unbinding and disableing vao and vbo
		GLCall(glBindVertexArray(0));
	}
	void SpriteBatch::sortGlyphs()
	{
		switch (sortType) {
		case ge::GlyphSortType::FRONT_TO_BACK:
			std::stable_sort(glyphPtrs.begin(), glyphPtrs.end(),
				[](Glyph* a, Glyph* b) {return a->depth < b->depth; });
			break;
		case ge::GlyphSortType::BACK_TO_FRONT:
			std::stable_sort(glyphPtrs.begin(), glyphPtrs.end(),
				[](Glyph* a, Glyph* b) {return a->depth > b->depth; });
			break;
		case ge::GlyphSortType::TEXTURE:
			std::stable_sort(glyphPtrs.begin(), glyphPtrs.end(),
				[](Glyph* a, Glyph* b) {return a->m_texture < b->m_texture; });
			break;
		}

	}

} //namespace ge
