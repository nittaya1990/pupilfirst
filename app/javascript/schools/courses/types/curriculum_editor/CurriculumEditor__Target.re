exception InvalidVisibilityValue(string);
exception InvalidRoleValue(string);

type role =
  | Student
  | Team;

type visibility =
  | Draft
  | Live
  | Archived;

type t = {
  id: string,
  role,
  targetGroupId: string,
  title: string,
  evaluationCriteria: list(int),
  prerequisiteTargets: list(int),
  quiz: list(CurriculumEditor__QuizQuestion.t),
  linkToComplete: option(string),
  sortIndex: int,
  visibility,
  completionInstructions: option(string),
};

let id = t => t.id;

let title = t => t.title;

let completionInstructions = t => t.completionInstructions;

let targetGroupId = t => t.targetGroupId;

let evaluationCriteria = t => t.evaluationCriteria;

let prerequisiteTargets = t => t.prerequisiteTargets;

let quiz = t => t.quiz;

let linkToComplete = t => t.linkToComplete;

let sortIndex = t => t.sortIndex;

let visibility = t => t.visibility;

let role = t => t.role;

let decodeVisbility = visibilityString =>
  switch (visibilityString) {
  | "draft" => Draft
  | "live" => Live
  | "archived" => Archived
  | _ => raise(InvalidVisibilityValue("Unknown Value"))
  };

let decodeRole = roleString =>
  switch (roleString) {
  | "student" => Student
  | "team" => Team
  | role => raise(InvalidRoleValue("Unknown Value :" ++ role))
  };

let decode = json =>
  Json.Decode.{
    id: json |> field("id", string),
    targetGroupId: json |> field("targetGroupId", string),
    title: json |> field("title", string),
    evaluationCriteria: json |> field("evaluationCriteria", list(int)),
    prerequisiteTargets: json |> field("prerequisiteTargets", list(int)),
    quiz: json |> field("quiz", list(CurriculumEditor__QuizQuestion.decode)),
    linkToComplete:
      json |> field("linkToComplete", nullable(string)) |> Js.Null.toOption,
    sortIndex: json |> field("sortIndex", int),
    visibility: decodeVisbility(json |> field("visibility", string)),
    role: decodeRole(json |> field("role", string)),
    completionInstructions:
      json
      |> field("completionInstructions", nullable(string))
      |> Js.Null.toOption,
  };

let updateList = (targets, target) => {
  let oldTargets = targets |> List.filter(t => t.id !== target.id);
  oldTargets |> List.rev |> List.append([target]) |> List.rev;
};

let create =
    (
      ~id,
      ~targetGroupId,
      ~title,
      ~evaluationCriteria,
      ~prerequisiteTargets,
      ~quiz,
      ~linkToComplete,
      ~sortIndex,
      ~visibility,
      ~completionInstructions,
      ~role,
    ) => {
  id,
  targetGroupId,
  title,
  evaluationCriteria,
  prerequisiteTargets,
  quiz,
  linkToComplete,
  sortIndex,
  visibility,
  completionInstructions,
  role,
};

let sort = targets =>
  targets |> List.sort((x, y) => x.sortIndex - y.sortIndex);

let archive = t => {...t, visibility: Archived};

let archived = t => {
  switch (t.visibility) {
  | Archived => true
  | Live => false
  | Draft => false
  };
};

let removeTarget = (target, targets) =>
  targets |> List.filter(t => t.id != target.id);

let targetIdsInTargetGroup = (id, targets) =>
  targets |> List.filter(t => t.targetGroupId == id) |> List.map(t => t.id);

let updateSortIndex = sortedTargets =>
  sortedTargets
  |> List.mapi((sortIndex, t) =>
       create(
         ~id=t.id,
         ~targetGroupId=t.targetGroupId,
         ~title=t.title,
         ~evaluationCriteria=t.evaluationCriteria,
         ~prerequisiteTargets=t.prerequisiteTargets,
         ~quiz=t.quiz,
         ~linkToComplete=t.linkToComplete,
         ~sortIndex,
         ~visibility=t.visibility,
         ~completionInstructions=t.completionInstructions,
         ~role=t.role,
       )
     );

let template = (id, targetGroupId, title) => {
  create(
    ~id,
    ~targetGroupId,
    ~title,
    ~evaluationCriteria=[],
    ~prerequisiteTargets=[],
    ~quiz=[],
    ~linkToComplete=None,
    ~sortIndex=999,
    ~visibility=Draft,
    ~completionInstructions=None,
    ~role=Student,
  );
};
