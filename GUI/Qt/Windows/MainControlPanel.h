#ifndef MAINCONTROLPANEL_H
#define MAINCONTROLPANEL_H

#include <SNAPComponent.h>

namespace Ui {
class MainControlPanel;
}

class GlobalUIModel;

class MainControlPanel : public SNAPComponent
{
  Q_OBJECT
  
public:
  explicit MainControlPanel(QWidget *parent = 0);
  ~MainControlPanel();

  void SetModel(GlobalUIModel *model);

protected slots:

  virtual void onModelUpdate(const EventBucket &bucket);

private slots:

  void on_btnCursorInspector_clicked(bool checked);

  void on_btnZoomInspector_clicked(bool checked);

  void on_btnLabelInspector_clicked(bool checked);

  void on_btnDisplayInspector_clicked(bool checked);

  void on_btnSyncInspector_clicked(bool checked);

  void on_btnToolInspector_clicked(bool checked);

  void on_actionCrosshair_triggered(bool checked);

  void on_actionZoomPan_triggered(bool checked);

  void on_actionPolygon_triggered(bool checked);

  void on_actionPaintbrush_triggered(bool checked);

  void on_actionSnake_triggered(bool checked);

private:
  Ui::MainControlPanel *ui;
  GlobalUIModel *m_Model;
};

#endif // MAINCONTROLPANEL_H