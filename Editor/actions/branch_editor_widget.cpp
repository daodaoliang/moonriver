/* Copyright (C) 2012, 2013 Carlos Pais 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "branch_editor_widget.h"

#include <QComboBox>
#include <QDebug>

#include "combobox.h"
#include "condition_text_edit.h"
#include "add_action_dialog.h"

BranchEditorWidget::BranchEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{

    mConditionEdit = new ConditionTextEdit(this);

    //connect(mConditionsTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(onConditionsClicked(QTreeWidgetItem*, int)));

    mConditionEdit->installEventFilter(this);
    mConditionEdit->setMaximumHeight(50);
    connect(mConditionEdit, SIGNAL(textChanged()), this, SLOT(onConditionChanged()));

    beginGroup(tr("Condition"));
    appendRow(tr("Contidition"), mConditionEdit);
    endGroup();

    beginGroup(tr("If True"));
    mTrueActionsChooser = new ComboBox(this);
    mTrueActionsChooser->setObjectName("true");
    appendRow(tr("Action"), mTrueActionsChooser);
    connect(mTrueActionsChooser, SIGNAL(addItemActivated()), this, SLOT(onAddItemActivated()));
    connect(mTrueActionsChooser, SIGNAL(itemActivated(int)), this, SLOT(onItemActivated(int)));
    connect(mTrueActionsChooser, SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
    endGroup();

    beginGroup(tr("If False"));
    mFalseActionsChooser = new ComboBox(this);
    mFalseActionsChooser->setObjectName("false");
    appendRow(tr("Action"), mFalseActionsChooser);
    connect(mFalseActionsChooser, SIGNAL(addItemActivated()), this, SLOT(onAddItemActivated()));
    connect(mFalseActionsChooser, SIGNAL(itemActivated(int)), this, SLOT(onItemActivated(int)));
    connect(mFalseActionsChooser, SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
    endGroup();

    resizeColumnToContents(0);

    mCurrentBranch = 0;
}

void BranchEditorWidget::updateData(Action * action)
{
    ActionEditorWidget::updateData(action);

    mTrueActionsChooser->clear();
    mFalseActionsChooser->clear();

    mCurrentBranch = qobject_cast<Branch*>(action);
    if (! mCurrentBranch)
        return;

    mConditionEdit->setText(mCurrentBranch->condition());

    QList<Action*> actions = mCurrentBranch->actions(true);
    foreach(Action* action,  actions)
        mTrueActionsChooser->addItem(action->icon(), action->toString());

    actions = mCurrentBranch->actions(false);
    foreach(Action* action,  actions)
        mFalseActionsChooser->addItem(action->icon(), action->toString());
}

void BranchEditorWidget::onConditionsClicked()
{
}

bool BranchEditorWidget::eventFilter(QObject * obj, QEvent * event)
{
    if (obj == mConditionEdit && event->type() == QEvent::MouseButtonDblClick)
        onConditionsClicked();

    return ActionEditorWidget::eventFilter(obj, event);
}

void BranchEditorWidget::onAddItemActivated()
{
    if (! mCurrentBranch)
        return;

    AddActionDialog dialog(Interaction::MouseRelease);
    dialog.exec();

    if (dialog.result() == QDialog::Accepted && dialog.selectedAction()) {

        Action* action = dialog.selectedAction();
        ComboBox *comboBox = qobject_cast<ComboBox*>(sender());
        if(comboBox && action) {
            comboBox->addItem(action->icon(), action->toString());
            if (sender()->objectName() == "true")
                mCurrentBranch->appendAction(action, true);
            else
                mCurrentBranch->appendAction(action, false);
        }
    }
}

void BranchEditorWidget::onItemActivated(int index)
{
    Action* action = 0;

    if (sender()->objectName() == "true")
        action = mCurrentBranch->action(index, true);
    else
        action = mCurrentBranch->action(index, false);

    if (! action)
        return;

    AddActionDialog dialog(action);
    dialog.exec();

    //update action's text
    if (sender()->objectName() == "true")
        mTrueActionsChooser->setItemText(index, action->toString());
    else
        mFalseActionsChooser->setItemText(index, action->toString());
}

void BranchEditorWidget::onItemRemoved(int index)
{
    if (sender()->objectName() == "true")
        mCurrentBranch->removeAction(index, true, true);
    else
        mCurrentBranch->removeAction(index, false, true);
}

void BranchEditorWidget::onConditionChanged()
{
    if (mCurrentBranch)
        mCurrentBranch->setCondition(mConditionEdit->toPlainText());
}
