#include "UndoRedo.hpp"
#include "../../Logger.hpp"
#include "../Room.hpp"

namespace UndoRedo {
	
	// ============================================================================
	// ROOM POSITION ACTION IMPLEMENTATION
	// ============================================================================
	
	RoomPositionAction::RoomPositionAction(
		const std::map<Room*, RoomPositionData>& before,
		const std::map<Room*, RoomPositionData>& after,
		const std::string& desc
	) : beforeState(before), afterState(after), description(desc) {}
	
	void RoomPositionAction::undo() {
		// Restore previous positions
		for (auto& pair : beforeState) {
			Room* room = pair.first;
			RoomPositionData& pos = pair.second;
			room->canonPosition = pos.canonPosition;
			room->devPosition = pos.devPosition;
		}
		
		if (debugMode) {
			Logger::info("[UndoRedo] Undo '", description, "' - Restored ", 
			             beforeState.size(), " rooms");
		}
	}
	
	void RoomPositionAction::redo() {
		// Restore next positions
		for (auto& pair : afterState) {
			Room* room = pair.first;
			RoomPositionData& pos = pair.second;
			room->canonPosition = pos.canonPosition;
			room->devPosition = pos.devPosition;
		}
		
		if (debugMode) {
			Logger::info("[UndoRedo] Redo '", description, "' - Restored ", 
			             afterState.size(), " rooms");
		}
	}
	
	// ============================================================================
	// UNDO/REDO MANAGER IMPLEMENTATION
	// ============================================================================
	
	void UndoRedoManager::pushAction(std::shared_ptr<IAction> action) {
		undoStack.push(action);
		clearRedoStack();
		
		if (debugMode) {
			Logger::info("[UndoRedo] Action pushed - Type: '", action->getTypeName(),
			             "' | Description: '", action->getDescription(),
			             "' | Undo stack: ", undoStack.size());
		}
	}
	
	bool UndoRedoManager::undo() {
		if (undoStack.empty()) {
			if (debugMode) {
				Logger::warn("[UndoRedo] No actions to undo");
			}
			return false;
		}
		
		// Get current action
		std::shared_ptr<IAction> action = undoStack.top();
		undoStack.pop();
		
		// Execute undo
		action->undo();
		
		// Move to redo stack
		redoStack.push(action);
		
		if (debugMode) {
			Logger::info("[UndoRedo] Undo executed - Type: '", action->getTypeName(),
			             "' | Undo stack: ", undoStack.size(),
			             " | Redo stack: ", redoStack.size());
		}
		
		return true;
	}
	
	bool UndoRedoManager::redo() {
		if (redoStack.empty()) {
			if (debugMode) {
				Logger::warn("[UndoRedo] No actions to redo");
			}
			return false;
		}
		
		// Get next action
		std::shared_ptr<IAction> action = redoStack.top();
		redoStack.pop();
		
		// Execute redo
		action->redo();
		
		// Move back to undo stack
		undoStack.push(action);
		
		if (debugMode) {
			Logger::info("[UndoRedo] Redo executed - Type: '", action->getTypeName(),
			             "' | Undo stack: ", undoStack.size(),
			             " | Redo stack: ", redoStack.size());
		}
		
		return true;
	}
	
	void UndoRedoManager::clearRedoStack() {
		int size = redoStack.size();
		while (!redoStack.empty()) {
			redoStack.pop();
		}
		
		if (debugMode && size > 0) {
			Logger::info("[UndoRedo] Redo stack cleared (", size, " actions removed)");
		}
	}
	
	void UndoRedoManager::clear() {
		while (!undoStack.empty()) undoStack.pop();
		while (!redoStack.empty()) redoStack.pop();
		
		if (debugMode) {
			Logger::info("[UndoRedo] All stacks cleared");
		}
	}
	
	std::string UndoRedoManager::getLastActionDescription() const {
		if (undoStack.empty()) {
			return "N/A";
		}
		return undoStack.top()->getDescription();
	}
	
	// ============================================================================
	// GLOBAL INSTANCES
	// ============================================================================
	
	UndoRedoManager manager;
	bool debugMode = true;
	
	// ============================================================================
	// CONVENIENCE FUNCTIONS
	// ============================================================================
	
	void init() {
		manager.clear();
		
		if (debugMode) {
			Logger::info("[UndoRedo] System initialized");
		}
	}
	
	void cleanup() {
		manager.clear();
		
		if (debugMode) {
			Logger::info("[UndoRedo] System cleaned up");
		}
	}
	
	void pushRoomPositionSnapshot(
		const std::map<Room*, RoomPositionData>& beforePositions,
		const std::map<Room*, RoomPositionData>& afterPositions,
		const std::string& description
	) {
		auto action = std::make_shared<RoomPositionAction>(
			beforePositions, 
			afterPositions, 
			description
		);
		manager.pushAction(action);
	}
	
	bool undo() {
		return manager.undo();
	}
	
	bool redo() {
		return manager.redo();
	}
	
	int getUndoStackSize() {
		return manager.getUndoStackSize();
	}
	
	int getRedoStackSize() {
		return manager.getRedoStackSize();
	}
	
	std::string getLastAction() {
		return manager.getLastActionDescription();
	}
	
}
