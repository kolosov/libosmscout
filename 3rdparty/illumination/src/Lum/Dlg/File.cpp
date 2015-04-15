#include <Lum/Dlg/File.h>

#include <Lum/Private/Config.h>

#if defined(HAVE_UNISTD_H)
  #include <unistd.h>
#endif

#include <Lum/Base/L10N.h>
#include <Lum/Base/Path.h>
#include <Lum/Base/String.h>
#include <Lum/Base/Util.h>

#include <Lum/Manager/FileSystem.h>

#include <Lum/Model/Selection.h>
#include <Lum/Model/Table.h>

#include <Lum/Button.h>
#include <Lum/ButtonRow.h>
#include <Lum/Combo.h>
#include <Lum/DirSelect.h>
#include <Lum/Menu.h>
#include <Lum/Panel.h>
#include <Lum/Popup.h>
#include <Lum/PopupGroup.h>

namespace Lum {
  namespace Dlg {
    struct RootsEntry
    {
      std::wstring name;
      std::wstring path;

      RootsEntry(const std::wstring& name, const std::wstring& path)
      : name(name),
        path(path)
      {
        // no code
      }
    };

    typedef Model::StdTable<RootsEntry> RootsModel;
    typedef Base::Reference<RootsModel> RootsModelRef;

    class RootsModelPainter : public StringCellPainter
    {
    public:
      std::wstring GetCellData() const
      {
        const RootsEntry entry=dynamic_cast<const RootsModel*>(GetModel())->GetEntry(GetRow());

        switch (GetColumn()) {
        case 1:
          return entry.name;
        case 2:
          return entry.path;
        default:
          return L"";
        }
      }
    };

    class Locations : public Popup
    {
    public:
      std::wstring                  result;
      Table                         *table;
      Model::SingleLineSelectionRef selection;
      RootsModelRef                 roots;

    public:
      Locations()
      : table(new Table()),
        selection(new Model::SingleLineSelection),
        roots(new RootsModel())
      {
        Observe(table->GetTableView()->GetMouseSelectionAction());
      }

      void PreInit()
      {
        std::list<Manager::FileSystem::Place> places;

        Manager::FileSystem::Instance()->GetPlaces(places);

        Model::HeaderRef headerModel=new Model::HeaderImpl();
        headerModel->AddColumn(L"",Lum::Base::Size::pixel,0);

        for (std::list<Manager::FileSystem::Place>::const_iterator place=places.begin();
             place!=places.end();
             ++place) {
          roots->Append(RootsEntry(place->description,
                                   place->path));
        }

        table->SetFlex(true,true);
        table->SetMinHeight(Base::Size::stdCharHeight,1);
        table->SetMaxWidth(Base::Size::workVRel,40);
        table->SetMaxHeight(Base::Size::workVRel,40);
        table->SetHeaderModel(headerModel);
        table->SetSelection(selection);
        table->SetModel(roots);
        table->SetPainter(new RootsModelPainter());
        table->SetAutoFitColumns(true);
        table->SetAutoHSize(true);
        table->SetAutoVSize(true);
        RegisterCommitShortcut(table->GetTableView(),
                               table->GetTableView()->GetMouseSelectionAction());

        SetMain(table);

        Popup::PreInit();
      }

      void Resync(Base::Model* model, const Base::ResyncMsg& msg)
      {
        if (model==table->GetTableView()->GetMouseSelectionAction() && table->GetTableView()->GetMouseSelectionAction()->IsFinished()) {
          if (selection->HasSelection()) {
            result=roots->GetEntry(selection->GetLine()).path;
          }
          Exit();
        }

        Popup::Resync(model,msg);
      }
    };

    File::Options::Options()
    : existingOnly(false),mode(modeFile),defaultFilter(0)
    {
      // no code
    }

    File::Options::~Options()
    {
      for (size_t x=0; x<filters.size(); ++x) {
        delete filters[x];
      }
    }

    void File::Options::SetMode(Mode mode)
    {
      this->mode=mode;
    }

    void File::Options::SetExistingOnly(bool exists)
    {
      existingOnly=exists;
    }

    void File::Options::AddFilter(Model::Dir::Filter* filter)
    {
      filters.push_back(filter);
    }

    void File::Options::SetDefaultFilter(size_t pos)
    {
      defaultFilter=pos;
    }

    void File::Options::SetDefaultFilter(const std::wstring& name)
    {
      for (defaultFilter=0; defaultFilter<filters.size(); ++defaultFilter) {
        if (filters[defaultFilter]->GetName()==name) {
          return;
        }
      }

      defaultFilter=0;
    }

    class DirModelPainter : public StringCellPainter
    {
    public:
      DirModelPainter()
      {
        SetAlignment(1,left);
        SetAlignment(2,right);
        SetAlignment(3,right);
      }

      std::wstring GetCellData() const
      {
        Base::DirScanner::Entry entry;

        dynamic_cast<const Model::Dir*>(GetModel())->GetEntry(GetRow(),entry);

        switch (GetColumn()) {
        case 1:
          return entry.name;
        case 2:
          if (entry.isDir) {
            return _l(L"MODEL_DIR_DIR",L"(dir)");
          }
          else {
            return Base::ByteSizeToWString(entry.size);
          }
        case 3:
          if (entry.timeModification.IsToday()) {
            return entry.timeModification.GetLocalLocaleTime();
          }
          else {
            return entry.timeModification.GetLocalLocaleDate();
          }
        default:
          return L"";
        }
      }
    };

    File::File()
    : dirList(new Model::Dir()),
      path(new Model::Path()),
      full(new Model::String()),
      filter(new Model::ULong()),
      showHidden(new Model::Boolean()),
      okAction(new Model::Action()),
      reloadAction(new Model::Action()),
      parentAction(new Model::Action()),
      fullAction(new Model::Action()),
      doubleClickAction(new Model::Action()),
      locationsAction(new Model::Action()),
      selection(new Model::SingleLineSelection()),
      table(NULL),
      result(false)
    {
      Base::Path tmp;

      tmp.SetNativeDir(Base::Path::GetCWD());

      // We switch notification of, since we do not want to trigger
      // a call to Load() yet...
      path->Off();

      path->Set(tmp);
      full->Set(Base::Path::GetCWD());

      // We switch notification of, since we do not want to trigger
      // a call to Load() yet...
      showHidden->Off();
      showHidden->Set(false);

      Observe(GetOpenedAction());
      Observe(GetClosedAction());
      Observe(path);
      Observe(filter);
      Observe(showHidden);
      Observe(okAction);
      Observe(reloadAction);
      Observe(parentAction);
      Observe(fullAction);
      Observe(doubleClickAction);
      Observe(locationsAction);
      Observe(selection);
    }

    File::~File()
    {
      // no code
    }

    void File::SetOptions(Options* options)
    {
      this->options=options;

      if (options->filters.size()>0) {
        filter->Set(options->defaultFilter+1);
      }
      else {
        filter->SetNull();
      }
    }

    bool File::GetResult() const
    {
      return result;
    }

    Lum::Object* File::GetContent()
    {
      DirSelect        *dirSel;
      Menu             *popup;
      Panel            *hPanel,*vPanel;
      Model::HeaderRef header;

      vPanel=VPanel::Create(true,true);

      if (!Lum::OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        dirSel=new DirSelect();
        dirSel->SetFlex(true,false);
        dirSel->SetModel(path);
        vPanel->Add(dirSel);

        vPanel->AddSpace();
      }

      header=new Model::HeaderImpl();
      header->AddColumn(_l(L"MODEL_DIR_FILENAME",L"Filename"),Base::Size::stdCharWidth,15,true);
      header->AddColumn(_l(L"MODEL_DIR_SIZE",L"Size"),Base::Size::stdCharWidth,std::max((size_t)8u,_l(L"MODEL_DIR_DIR",L"(dir)").length()));
      header->AddColumn(_l(L"MODEL_DIR_DATE",L"Date"),Base::Size::stdCharWidth,10);

      table=new Table();
      table->SetFlex(true,true);
      table->SetMinWidth(Base::Size::stdCharWidth,40);
      table->SetShowHeader(!Lum::OS::display->GetTheme()->RequestFingerFriendlyControls());
      table->GetTableView()->SetAutoFitColumns(true);
      table->GetTableView()->SetAutoHSize(true);
      table->SetSelection(selection);
      table->SetDoubleClickAction(doubleClickAction);
      table->SetModel(dirList);
      table->SetHeaderModel(header);
      table->SetPainter(new DirModelPainter());

      popup=new Menu();
      popup->SetParent(GetWindow());
      popup->AddBoolItem(_l(L"DLG_FILE_SHOW_HIDDEN",
                            L"Show _hidden files"),
                            showHidden,
                            OS::qualifierControl,L"h");
      table->SetMenu(popup->GetWindow());

      vPanel->Add(table);
      vPanel->AddSpace();

      hPanel=HPanel::Create(true,false);

      if (!Lum::OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        locations=Button::Create(_l(L"DLG_FILE_LOCATIONS",L"_Locations"),
                                 locationsAction,false,true);
        hPanel->Add(locations);
      }

      hPanel->AddSpace(true);

      if (options->filters.size()>0 &&
          (!Lum::OS::display->GetTheme()->RequestFingerFriendlyControls() ||
           options->filters.size()>1)) {
        Combo              *combo;
        Model::StringTable *filters;

        filters=new Model::StringTable();
        for (size_t x=0; x<options->filters.size(); ++x) {
          filters->Append(options->filters[x]->GetName());
        }

        combo=new IndexCombo();
        combo->SetWidth(Base::Size::stdCharWidth,15);
        combo->SetModel(filter);
        combo->SetTableModel(filters);
        hPanel->Add(combo);
      }

      vPanel->Add(hPanel);
      vPanel->AddSpace();

      hPanel=HPanel::Create(true,false);

      if (Lum::OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        hPanel->Add(Button::Create(_l(L"DLG_FILE_UP",L".."),parentAction,false,true));
      }

      fullStr=String::Create(full,true,false);
      fullStr->SetReturnAction(fullAction);
      hPanel->Add(fullStr);

      if (!Lum::OS::display->GetTheme()->RequestFingerFriendlyControls()) {
        hPanel->Add(Button::Create(_l(L"DLG_FILE_RELOAD",L"_Reload"),reloadAction,false,true));
      }

      vPanel->Add(hPanel);

      if (OS::display->GetSize()<OS::Display::sizeNormal) {
        vPanel->SetMinHeight(Base::Size::workVRel,80);
      }
      else {
        vPanel->SetMinHeight(Base::Size::workVRel,60);
      }

      RegisterCommitShortcut(table->GetTableView(),okAction);
      RegisterShortcut(table->GetTableView(),0,L"BackSpace",parentAction);

      return vPanel;
    }

    void File::GetActions(std::vector<ActionInfo>& actions)
    {
      CreateActionInfosOkCancel(actions,okAction,GetClosedAction());
    }

    void File::SetPath(const std::wstring& path)
    {
      if (path.empty()) {
        return;
      }

      if (options->existingOnly &&
          !Base::Path::Exists(path)) {
        return;
      }

      Base::Path newPath;

      if (Base::Path::IsDir(path)) {
        std::wstring dir;

        if (Base::Path::GetAbsoluteFilename(path,dir)) {
          newPath.SetNativeDir(dir);
        }
      }
      else {
        std::wstring file;

        if (Base::Path::GetAbsoluteFilename(path,file)) {
          newPath.SetNativePath(file);
        }
      }

      this->path->Set(newPath);
    }

    void File::Load()
    {
      dirList->Off();
      table->GetTableView()->GetSelection()->Clear();
      dirList->SetDirectory(path->Get().GetDir(),
                            !showHidden->Get(),
                            options->mode==Options::modeDirectory);

      if (table->GetTableView()->GetVAdjustment()->IsValid()) {
        table->GetTableView()->GetVAdjustment()->SetTop(1);
      }
      dirList->On();


      // Later we should only avoid selection if the path is a file,
      // the file does not exist and choosing non-existing files is allowed
      if (GetOpenedAction()->IsFinished() &&
          !path->Get().IsEmpty() &&
          !path->Get().GetBaseName().empty()) {
        return;
      }

      // TODO: If path is not empty, try to select it in existing list.
      if (dirList->GetRows()>0) {
        dynamic_cast<Model::SingleLineSelection*>(table->GetTableView()->GetSelection())->SelectLine(1);
      }
    }

    void File::GotoParent()
    {
      if (path->Get().IsRoot()) {
        return;
      }

      Base::Path newPath(path->Get());

      newPath.GoUp();

      path->Set(newPath);
    }

    void File::GotoChildDirectory(const std::wstring& dir)
    {
      Base::Path newPath(path->Get());

      newPath.AppendDir(dir);

      path->Set(newPath);
    }

    bool File::GetSelection(Base::DirScanner::Entry& entry) const
    {
      size_t line;

      if (table!=NULL &&
          table->GetTableView()!=NULL &&
          table->GetTableView()->GetSelection()->HasSelection()) {
        line=dynamic_cast<Model::SingleLineSelection*>(table->GetTableView()->GetSelection())->GetLine();

        dirList->GetEntry(line,entry);
        return true;
      }
      else {
        return false;
      }
    }

    /**
      This callback is called, if the selection is the file/dir box changes.
    */
    void File::OnSelectionChange()
    {
      Base::DirScanner::Entry entry;

      if (!GetSelection(entry)) {
        return;
      }

      Base::Path newPath(path->Get());

      if (options->mode==Options::modeDirectory) {
        if (entry.isDir) {
          newPath.AppendDir(entry.name);
          full->Set(newPath.GetDir(true));
        }
      }
      else {
        if (entry.isDir) {
          full->Set(newPath.GetDir(true));
        }
        else {
          newPath.SetBaseName(entry.name);
          full->Set(newPath.GetPath());
        }
      }
    }

    /**
      This callback is called, if "return" gets triggered while editing
      (focusing) the edit field below the file/dir box.

      It uses this->full->Get() as source for actions.
    */
    void File::HandleChange()
    {
      std::wstring full;

      full=this->full->Get();

      if (full.empty()) {
        // Someone has completely deleted string entry field
        this->full->Set(path->Get().GetPath());
        return;
      }

      if (!Base::Path::Exists(full) && options->existingOnly) {
        this->full->Set(path->Get().GetPath());
        return;
      }

      Base::Path newPath(full);

      if (options->mode==Options::modeDirectory) {
        if (newPath.IsDir() || Base::Path::IsDir(full)) {
          result=true;
          Exit();
        }
        else {
          newPath.SetBaseName(L"");
          path->Set(newPath);
        }
      }
      else {
        if (newPath.IsDir() || Base::Path::IsDir(full)) {
          newPath.SetNativeDir(full);
          path->Set(newPath);
        }
        else {
          result=true;
          Exit();
        }
      }
    }

    /**
      This callback is called, if the OK button is clicked or selected by
      pressing the key shortcut "return".

      It uses this->path->Get() together with the current selection
      as source for actions.
    */
    void File::OnOk()
    {
      Base::Path newPath(full->Get());

      if (options->mode==Options::modeDirectory) {
        if (newPath.Exists() && newPath.IsDir()) {
          result=true;
          Exit();
        }
      }
      else {
        if (!options->existingOnly) {
          result=true;
          Exit();
        }
        else if (options->existingOnly &&
                 newPath.Exists() && !newPath.IsDir()) {
          result=true;
          Exit();
        }
        else {
          Base::DirScanner::Entry entry;

          if (GetSelection(entry)) {
            if (entry.isDir) {
              GotoChildDirectory(entry.name);
            }
          }
        }
      }
    }

    void File::OnDoubleClick()
    {
      Base::DirScanner::Entry entry;

      if (GetSelection(entry)) {
        if (entry.isDir) {
          GotoChildDirectory(entry.name);
        }
        else {
          OnOk();
        }
      }
    }

    void File::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      if (model==path) {
        full->Set(path->Get().GetPath());
        Load();
      }
      if (model==showHidden) {
        Load();
      }
      else if (model==okAction  &&
               okAction->IsFinished()) {
        OnOk();
      }
      else if (model==GetClosedAction() &&
               GetClosedAction()->IsFinished()) {
        result=false;
        Exit();
      }
      else if (model==GetOpenedAction() &&
               GetOpenedAction()->IsFinished()) {
        path->On();
        showHidden->On(false);
        SetFocus(table->GetTableView());
      }
      else if (model==reloadAction &&
               reloadAction->IsFinished()) {
        Load();
      }
      else if (model==parentAction &&
               parentAction->IsFinished()) {
        GotoParent();
      }
      else if (model==fullAction &&
               fullAction->IsFinished()) {
        HandleChange();
      }
      else if (selection->HasSelectionChanged(model,msg)) {
        OnSelectionChange();
      }
      else if (model==doubleClickAction &&
               doubleClickAction->IsFinished()) {
        OnDoubleClick();
      }
      else if (model==locationsAction &&
               locationsAction->IsFinished()) {
        Locations *popup;

        popup=new Locations();
        popup->SetParent(GetWindow());
        popup->SetReference(locations);

        if (popup->Open()) {
          popup->EventLoop();
          popup->Close();
        }

        if (!popup->result.empty()) {
          Base::Path tmp;

          tmp.SetNativeDir(popup->result);
          path->Set(tmp);
        }

        delete popup;
       }
      else if (model==filter &&
               dirList.Valid() &&
               options.Valid()) {
        dirList->SetFilter(options->filters[filter->GetUnsignedLong()-1]);
        if (table!=NULL) {
          Load();
        }
      }

      Dlg::ActionDialog::Resync(model,msg);
    }

    std::wstring File::GetPath() const
    {
      return full->Get();
    }

    bool File::GetFile(OS::Window* parent,
                       const std::wstring& title,
                       std::wstring& file,
                       Options* options)
    {
      File dialog;
      bool result=false;

      if (options==NULL) {
        options=new Options();
      }
      options->SetMode(Options::modeFile);
      options->SetExistingOnly(true);

      dialog.SetParent(parent);
      dialog.SetTitle(title);
      dialog.SetOptions(options);
      dialog.SetPath(file);

      if (dialog.Open()) {
        dialog.EventLoop();
        dialog.Close();

        if (dialog.GetResult()) {
          file=dialog.GetPath();
          result=true;
        }
      }

      return result;
    }

    bool File::SaveFile(OS::Window* parent,
                        const std::wstring& title,
                        std::wstring& file,
                        Options* options)
    {
      File *dialog;
      bool result=false;

      if (options==NULL) {
        options=new Options();
      }

      options->SetMode(Options::modeFile);
      options->SetExistingOnly(false);

      dialog=new File();
      dialog->SetParent(parent);
      dialog->SetTitle(title);
      dialog->SetOptions(options);
      dialog->SetPath(file);

      if (dialog->Open()) {
        dialog->EventLoop();
        dialog->Close();

        if (dialog->GetResult()) {
          file=dialog->GetPath();
          result=true;
        }
      }

      delete dialog;

      return result;
    }

    bool File::GetDir(OS::Window* parent,
                      const std::wstring& title,
                      std::wstring& dir,
                      Options* options)
    {
      File dialog;
      bool result=false;

      if (options==NULL) {
        options=new Options();
      }

      options->SetMode(Options::modeDirectory);
      options->SetExistingOnly(true);

      dialog.SetParent(parent);
      dialog.SetTitle(title);
      dialog.SetOptions(options);
      dialog.SetPath(dir);

      if (dialog.Open()) {
        dialog.EventLoop();
        dialog.Close();

        if (dialog.GetResult()) {
          dir=dialog.GetPath();
          result=true;
        }
      }

      return result;
    }
  }
}
