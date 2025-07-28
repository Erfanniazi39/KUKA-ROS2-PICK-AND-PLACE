class Movement:

    @staticmethod
    def toward_to_object(x, y ,z):
        
        return {
            "Type": "MoveRP",
            "Speed": 1.0,
            "Input": {"x": x, "y": y, "z": 0.5, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}
        }
    
    @staticmethod
    def open_hand():
        
        return {
            "Type": "MoveG",
            "Speed": 1.0,
            "Input": {"value": 0.0} 
        }
    
    @staticmethod
    def approach_to_object(x, y, z):
        
        return{
            "Type": "MoveRP",
            "Speed": 0.2,
            "Input": {"x": x, "y": y, "z": 0.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}
        }

    @staticmethod
    def close_hand():
        
        return {
            "Type": "MoveG",
            "Speed": 1.0,
            "Input": {"value": 29.0}
        }
    
    @staticmethod
    def lift_object(x, y, z):

        return {
            "Type": "MoveRP",
            "Speed": 1.0,
            "Input": {"x": 1.5, "y": y, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}
        }   
    
    @staticmethod
    def carry_object(x, y, z):
        
        if y <= 0:
            return [
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": 0.0, "y": -1.5, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}},
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": -x, "y": y, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}}   
            ]
        else:
            return [
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": 0.0, "y": 1.5, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}},
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": -x, "y": y, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}} 
            ]
        
    @staticmethod
    def placing_object(x, y, z):
        
        return [
            {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": -x, "y": y, "z": 0.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}}
            
        ]
    
    @staticmethod
    def back_home(x, y, z):

        if y <= 0:
            return [
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": -x, "y": y, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}},
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": 0.0, "y": -1.5, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}},
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": 1.5, "y": 0.0, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}}
            ]
        else:
            return [
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": -x, "y": y, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}},
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": 0.0, "y": 1.5, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}},
                {"Type": "MoveRP", "Speed": 1.0, "Input": {"x": 1.5, "y": 0.0, "z": 1.3, "roll": 0.0, "pitch": 1.57, "yaw": 0.0}}
            ]
