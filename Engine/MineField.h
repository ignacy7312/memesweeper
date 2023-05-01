#pragma once
#include "Graphics.h"
#include "RectI.h"
#include "SpriteCodex.h"

class MineField {

private:
	class Tile {
	public:
		enum class State {
			Hidden,
			Flagged,
			Revealed
		};
		
	public:
		void SpawnBomb();
		bool HasBomb() const;
		void Draw(Graphics& gfx, bool fucked, Vei2& screenPos) const;
		bool IsRevealed() const;
		void Reveal();
		void ToggleFlag();
		bool IsFlagged() const;
		void SetNeighborBombCount(int bombCount);

	private:
		State state = State::Hidden;
		bool hasBomb = false;
		int nNeighborBombs = -1;
	};

public:
	MineField(int nBombs_in);
	RectI GetRect() const;

	void Draw(Graphics& gfx) const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos); 

private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	
	Vei2 ScreenToGrid(const Vei2& screenPos);
	
	int CountNeighborBombs(const Vei2& gridPos);

	bool IsWon() const;

private:
	static constexpr int width = 20;
	static constexpr int height = 16;

	static constexpr int boardTopLeftX = (Graphics::ScreenWidth / 2 - (width * SpriteCodex::tileSize) / 2);
	static constexpr int boardTopLeftY = (Graphics::ScreenHeight / 2 - (height * SpriteCodex::tileSize) / 2);
	static constexpr int borderWidth = 20;
	static constexpr Color borderColor = (192, 192, 192);

	Tile field[width * height];
	RectI bgRect = { boardTopLeftX, width * SpriteCodex::tileSize + boardTopLeftX, boardTopLeftY, height * SpriteCodex::tileSize + boardTopLeftY };
	RectI borderRect = bgRect.GetExpanded(borderWidth);
	int nBombs;
	int nBombsRemaining;
	int nBombsCorrectlyFlagged = 0;
	bool isFucked = false;

};