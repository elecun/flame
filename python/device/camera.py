
from abc import *
class camera(metaclass=ABCMeta):
    
    @abstractmethod
    def open(self):
        pass

    @abstractmethod
    def close(self):
        pass