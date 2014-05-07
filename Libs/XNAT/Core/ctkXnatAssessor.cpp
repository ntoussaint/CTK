/*=============================================================================

  Library: XNAT/Core

  Copyright (c) University College London,
    Centre for Medical Image Computing

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkXnatAssessor.h"

#include "ctkXnatSession.h"
#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatDefaultSchemaTypes.h"

#include "ctkXnatAssessorResource.h"
#include "ctkXnatScanResource.h"
#include <qDebug>

//----------------------------------------------------------------------------
class ctkXnatAssessorPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatAssessorPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }

  QString uri;
};


//----------------------------------------------------------------------------
ctkXnatAssessor::ctkXnatAssessor(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatAssessorPrivate(), parent, schemaType)
{
  qDebug() << " constructing  the assessor";
}

//----------------------------------------------------------------------------
ctkXnatAssessor::~ctkXnatAssessor()
{
}

//----------------------------------------------------------------------------
QString ctkXnatAssessor::resourceUri() const
{
  return QString("%1/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatAssessor::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatAssessor::fetchImpl()
{
  QString assessorResourcesUri = this->resourceUri() + "/resources";
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(assessorResourcesUri);

  QList<ctkXnatObject*> assessorResources = session->httpResults(queryId,
                                                             ctkXnatDefaultSchemaTypes::XSI_ASSESSOR_RESOURCE);

  foreach (ctkXnatObject* assessorResource, assessorResources)
  {
    QString label = assessorResource->property("label");
    if (!label.isEmpty())
    {
      assessorResource->setProperty("ID", label);
    }
    this->add(assessorResource);
  }

  assessorResourcesUri = this->resourceUri() + "/out/resources";
  queryId = session->httpGet(assessorResourcesUri);

  qDebug() << "retrieving assessor resources";
  assessorResources = session->httpResults(queryId,
					   ctkXnatDefaultSchemaTypes::XSI_ASSESSOR_RESOURCE);

  foreach (ctkXnatObject* assessorResource, assessorResources)
  {
    QString label = assessorResource->property("label");
    if (!label.isEmpty())
    {
      assessorResource->setProperty("ID", label);
      qDebug() << " adding object, is it AssessorResource : " << (typeid(ctkXnatScanResource) == typeid(assessorResource));
      qDebug() << " adding assessor resource object with properties \n: " << assessorResource->properties();
      
    }
    this->add(assessorResource);
  }
  
}