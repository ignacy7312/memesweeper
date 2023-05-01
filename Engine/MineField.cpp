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
	nBombsRemaining = nBombs;

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

	gfx.DrawRect(borderRect, borderColor);
	gfx.DrawRect(bgRect, SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; gridPos.y++) {
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++) {
			TileAt(gridPos).Draw(gfx, isFucked, gridPos * SpriteCodex::tileSize);
		}
	}
	if (IsWon()) {
		SpriteCodex::DrawWin(Vei2(254, 192), gfx);
	}
}

RectI MineField::GetRect() const
{
	return bgRect;
}

void MineField::OnRevealClick(const Vei2& screenPos)
{
	if (!isFucked && !IsWon()){
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed() && !tile.IsFlagged()) {
			//tile.Reveal();
			if (tile.HasBomb()) {
				tile.Reveal();
				isFucked = true;
			}
			else {
				Dig(gridPos);
			}
		}
	}
}

void MineField::OnFlagClick(const Vei2& screenPos)
{
	if (!isFucked && !IsWon()){
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed()) {
			tile.ToggleFlag();
		}
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
	Vei2 offset = { boardTopLeftX, boardTopLeftY };
	return (screenPos - offset) / SpriteCodex::tileSize;
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

void MineField::Dig(const Vei2& initPos)
{
	Tile& currentTile = TileAt(initPos);

	if (currentTile.HasBomb() || currentTile.IsFlagged()) {
		return;
	}

	

	if (currentTile.GetNumberNeighborBombs() > 0 && !currentTile.HasBomb() && !currentTile.IsFlagged()) {
		if (!currentTile.IsRevealed()) {
			currentTile.Reveal();
		}
		return;
	}
	
	if  (currentTile.GetNumberNeighborBombs() == 0 && !(currentTile.HasBomb() || currentTile.IsFlagged())) {
		if (!currentTile.IsRevealed()) {
			currentTile.Reveal();
		}
		int xStart = std::max(0, initPos.x - 1);
		int xEnd = std::min(width - 1, initPos.x + 1);
		int yStart = std::max(0, initPos.y - 1);
		int yEnd = std::min(height - 1, initPos.y + 1);

		for (Vei2 pos = { xStart, yStart }; pos.y <= yEnd; pos.y++) {
			for (pos.x = xStart; pos.x <= xEnd; pos.x++) {
				if (TileAt(pos).IsRevealed()) {
					continue;
				}
				else {
					Dig(pos);

				}
			}
		}
	}
}


bool MineField::IsWon() const
{
	int flagCount = 0;
	for (const Tile& tile : field) {
		if (tile.IsCorrectlyFlagged()) {
			flagCount++;
		}
	}
	return flagCount == nBombs;

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

void MineField::Tile::Draw(Graphics & gfx, bool fucked, Vei2& screenPos) const
{
	Vei2 offset(boardTopLeftX, boardTopLeftY);
	screenPos += offset;
	if (fucked){
		switch (state)
		{
		case State::Hidden:
			if (HasBomb()) {
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else {
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;

		case State::Flagged:
			if (HasBomb()) {
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else {
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (HasBomb()) {
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			else {
				SpriteCodex::DrawTileNumber(screenPos, nNeighborBombs, gfx);
			}
			break;
		}
	}
	else { //not fucked
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
				SpriteCodex::DrawTileNumber(screenPos, nNeighborBombs, gfx);
			}
			break;
		}
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
	correctlyFlagged = HasBomb();
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


int MineField::Tile::GetNumberNeighborBombs() const
{
	return nNeighborBombs;
}

bool MineField::Tile::IsCorrectlyFlagged() const
{
	return correctlyFlagged;
}
