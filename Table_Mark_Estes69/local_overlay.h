// git update-index --skip-worktree local_overlay.h to make your local mods
void local_overlay() {
  matrix->fillRect(0, 0, 64, 8, 0);
  // Reset to default adafruit font, that font wants cursor at top left corner
  matrix->setFont();
  matrix->setCursor(0, 0);
  matrix->setTextColor(matrix->Color(0,0,255));
  matrix->print("Marc");
  matrix->setCursor(28, 0);
  matrix->setTextColor(matrix->Color(0,255,0));
  matrix->print("MERLIN");
  matrix->fillRect(0, 55, 64, 9, 0);
  matrix->setCursor(0, 56);
  matrix->setTextColor(matrix->Color(255,255,0));
  matrix->print("SuperCon");
  matrix->setCursor(51, 56);
  matrix->setTextColor(matrix->Color(255,0,0));
  matrix->print("26");
}
