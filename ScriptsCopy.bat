del /q /f ".\External\Include\Scripts\*.*"
xcopy /s /y  /exclude:Scripts_exclude_list.txt ".\Project\Scripts\*.h" ".\External\Include\Scripts"