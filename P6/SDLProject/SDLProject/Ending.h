
#include "Scene.h"

class Ending : public Scene {
    
public:
    void Initialize(int totalLivesLeft) override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
