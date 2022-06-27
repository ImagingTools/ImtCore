// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtfile/IFileProvider.h>


namespace imtlicgql
{


class CFileDataControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFileDataControllerComp);
		I_ASSIGN(m_fileProviderCompPtr, "FileProvider", "File provider", false, "FileProvider");
		I_ASSIGN(m_fileNameAttrPtr, "FileName", "File name", false, "FileName");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtfile::IFileProvider, m_fileProviderCompPtr);
	I_TEXTATTR(m_fileNameAttrPtr);
};


} // namespace imtlicgql


