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
		void Draw(Graphics& gfx, const Vei2& screenPos) const;
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
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos); 

private:

	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2& screenPos);
	int CountNeighborBombs(const Vei2& gridPos);

	static constexpr int width = 20;
	static constexpr int height = 16;

	Tile field[width * height];
	int nBombs;
	
	RectI bgRect = { 0, width * SpriteCodex::tileSize, 0, height * SpriteCodex::tileSize };

};