#pragma once

#include <stack>
#include <string>
#include <memory>
#include <map>
#include "../../math/Vector.hpp"

class Room; // Forward declaration

namespace UndoRedo {

	// Abstract action interface, all undoable actions must implement this
	class IAction {
	public:
		virtual ~IAction() = default;
		
		virtual void undo() = 0;
		virtual void redo() = 0;
		
		virtual std::string getDescription() const = 0;
		virtual std::string getTypeName() const = 0;
	};

	class UndoRedoManager {
	private:
		std::stack<std::shared_ptr<IAction>> undoStack;
		std::stack<std::shared_ptr<IAction>> redoStack;
		
	public:

		void pushAction(std::shared_ptr<IAction> action);
		bool undo();
		bool redo();
		void clear();
		void clearRedoStack();
		
		int getUndoStackSize() const { return undoStack.size(); }
		int getRedoStackSize() const { return redoStack.size(); }
		
		// Get last action description
		std::string getLastActionDescription() const;
	};

    // ============================================================================
	// ROOM POSITION ACTION
	// Handles undo/redo for room position changes
	// ============================================================================
	
	struct RoomPositionData {
		Vector2 canonPosition;
		Vector2 devPosition;
		
		RoomPositionData() : canonPosition(0, 0), devPosition(0, 0) {}
		RoomPositionData(Vector2 canon, Vector2 dev) 
			: canonPosition(canon), devPosition(dev) {}
	};
	
	class RoomPositionAction : public IAction {
	private:
		std::map<Room*, RoomPositionData> beforeState;
		std::map<Room*, RoomPositionData> afterState;
		std::string description;
		
	public:
		RoomPositionAction(
			const std::map<Room*, RoomPositionData>& before,
			const std::map<Room*, RoomPositionData>& after,
			const std::string& desc
		);
		
		void undo() override;
		void redo() override;
		std::string getDescription() const override { return description; }
		std::string getTypeName() const override { return "RoomPosition"; }
	};
	
	// ============================================================================
	// GLOBAL MANAGER INSTANCE
	// ============================================================================
	
	extern UndoRedoManager manager;
	extern bool debugMode;
	
	// ============================================================================
	// CONVENIENCE FUNCTIONS
	// High-level API for common operations
	// ============================================================================
	
	// Initialize the system
	void init();
	
	// Cleanup the system
	void cleanup();
	
	// Push a room position snapshot
	void pushRoomPositionSnapshot(
		const std::map<Room*, RoomPositionData>& beforePositions,
		const std::map<Room*, RoomPositionData>& afterPositions,
		const std::string& description
	);
	
	// Execute undo
	bool undo();
	
	// Execute redo
	bool redo();
	
	// Get stack sizes
	int getUndoStackSize();
	int getRedoStackSize();
	
	// Get last action description
	std::string getLastAction();
	
}
