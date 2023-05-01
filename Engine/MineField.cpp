#include "MineField.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include "Graphics.h"
#include <assert.h>
#include <random>
#include <algorithm>

MineField::MineField(int nBombs_in)

{
	assert(nBombs_in > 0 && nBombs_in < width* height);
	nBombs = nBombs_in;

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);
	
	for (int nSpawned = 0; nSpawned < nBombs; nSpawned++) {
	
		Vei2 spawnPos;
		do {
			spawnPos = { xDist(rng), yDist(rng) };
		} while (TileAt(spawnPos).HasBomb());
		
		TileAt(spawnPos).SpawnBomb();
	}
	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++) {
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++) {
			TileAt(gridPos).SetNeighborBombCount(CountNeighborBombs(gridPos));
		}
	}

}

void MineField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(bgRect, SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++) {
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++) {
			TileAt(gridPos).Draw(gfx, gridPos * SpriteCodex::tileSize);
		}
	}
}

RectI MineField::GetRect() const
{
	return bgRect;
}

void MineField::OnRevealClick(const Vei2& screenPos)
{
	const Vei2 gridPos = ScreenToGrid(screenPos);
	assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed() && !tile.IsFlagged()) {
		tile.Reveal();
	}
}

void MineField::OnFlagClick(const Vei2& screenPos)
{
	const Vei2 gridPos = ScreenToGrid(screenPos);
	assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed()) {
		tile.ToggleFlag();
	}
}

MineField::Tile& MineField::TileAt(const Vei2& gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const MineField::Tile& MineField::TileAt(const Vei2& gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 MineField::ScreenToGrid(const Vei2& screenPos)
{
	return screenPos / SpriteCodex::tileSize;
}

int MineField::CountNeighborBombs(const Vei2& gridPos)
{
	int xStart = std::max(0, gridPos.x - 1);
	int xEnd = std::min(width - 1, gridPos.x + 1);
	int yStart = std::max(0, gridPos.y - 1);
	int yEnd = std::min(height - 1, gridPos.y + 1);

	int bombCounter = 0;
	for (Vei2 pos = { xStart, yStart }; pos.y <= yEnd; pos.y++) {
		for (pos.x = xStart; pos.x <= xEnd; pos.x++) {
			if (TileAt(pos).HasBomb()) {
				bombCounter++;
			}
		}
	}
	return bombCounter;
}

void MineField::Tile::SpawnBomb()
{
	assert(!hasBomb);
	hasBomb = true;
}

bool MineField::Tile::HasBomb() const
{
	return hasBomb;
}

void MineField::Tile::Draw(Graphics & gfx, const Vei2& screenPos) const
{
	switch (state)
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		break;
	case State::Flagged:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		SpriteCodex::DrawTileFlag(screenPos, gfx);
		break;
	case State::Revealed:
		if (hasBomb) {
			SpriteCodex::DrawTileBomb(screenPos, gfx);
		}
		else {
			SpriteCodex::DrawTileNumber(screenPos,nNeighborBombs, gfx);
		}
		break;
	}
}

bool MineField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

void MineField::Tile::Reveal()
{
	assert(!IsRevealed());
	state = State::Revealed;
}

void MineField::Tile::ToggleFlag()
{
	assert(!IsRevealed());
	if (state == State::Hidden) {
		state = State::Flagged;
	}
	else {
		state = State::Hidden;
	}
}

bool MineField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

void MineField::Tile::SetNeighborBombCount(int bombCount)
{
	assert(nNeighborBombs == -1);   
	nNeighborBombs = bombCount;
}
