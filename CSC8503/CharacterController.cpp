//#include "CharacterController.h"
//
//
//
//using namespace NCL::CSC8503;
//
//CharacterController::CharacterController(PaintballPhysicsObject* physObj, PerspectiveCamera* cam) {
//    this->physicsObject = physObj;
//    this->camera = cam;
//}
//    
//
//void CharacterController::MoveForward(float forceMagnitude) {
//    Vector3 fwd = CalculateForward();
//    physicsObject->AddForce(Util::NCLToRP3d(fwd * forceMagnitude));
//}
//
//void CharacterController::MoveBackward(float forceMagnitude) {
//    Vector3 fwd = CalculateForward();
//    physicsObject->AddForce(Util::NCLToRP3d(-fwd * forceMagnitude));
//}
//
//void CharacterController::MoveRight(float forceMagnitude) {
//    Vector3 right = CalculateRight();
//    physicsObject->AddForce(Util::NCLToRP3d(right * forceMagnitude));
//}
//
//void CharacterController::MoveLeft(float forceMagnitude) {
//    Vector3 right = CalculateRight();
//    physicsObject->AddForce(Util::NCLToRP3d(-right * forceMagnitude));
//}
//
//void CharacterController::Jump(float forceMagnitude) {
//    if (physicsObject->isStand()) {
//        physicsObject->ApplyLinearImpulse(rp3d::Vector3(0, forceMagnitude * 0.025f, 0));
//    }
// 
//}
//void CharacterController::GoDown(float forceMagnitude) {
//    physicsObject->AddForce(rp3d::Vector3(0, -forceMagnitude, 0));
// 
//}
//
//Vector3 CharacterController::CalculateForward() {
//    const Matrix4& view = camera->BuildViewMatrix();
//    const Matrix4& camWorld = Matrix::Inverse(view);
//    Vector3 right = Vector3(camWorld.GetColumn(0));
//    Vector3 forward = Vector::Cross(Vector3(0, 1, 0), right);
//    forward.y = 0; // Ensure the forward vector is horizontal
//    forward = Vector::Normalise(forward);
//    return forward;
//}
//
//Vector3 CharacterController::CalculateRight() {
//    const Matrix4& view = camera->BuildViewMatrix();
//    const Matrix4& camWorld = Matrix::Inverse(view);
//    return Vector3(camWorld.GetColumn(0)); // Right axis from camera world matrix
//}
